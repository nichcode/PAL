#!/usr/bin/env python3

import json
import html
import pickle
import re

from dataclasses import is_dataclass, fields, replace
from pathlib import Path

from jinja2 import Environment, FileSystemLoader
from models import (
    ConstantGroup,
    Function,
    Module,
    Page,
    Struct,
    Typedef,
    token_pattern,
    unesc,
    TOKENS
)

# Token key -> HTML
HTML_TEMPLATES = {
    "code": "<code>{content}</code>",
    "bold": "<strong>{content}</strong>",
    "ref": '<a href="{target_id}.html">{content}</a>',
    "codeblock": "<pre>{content}</pre>",
    "callout": '<div class="callout" type="{type}">{prefix}{content}</div>',
    "table": "<table>{content}</table>",
    "row": "<tr>{content}</tr>",
    "cell": "<td>{content}</td>",
    "para": "<p>{content}</p>",
    "link": '<a href="{url}" target="_blank" rel="noopener noreferrer">{content}</a>',
}

assert HTML_TEMPLATES.keys() == TOKENS.keys(), (
    "renderer is missing a token"
)

PATTERNS = {
    key: token_pattern(key)
    for key in TOKENS
}

BRIEF_LIMIT = 120


class HTMLRenderer:
    def __init__(self, root_dir, modules, pages: list[Page]):
        self.root_dir = root_dir
        loader = FileSystemLoader(root_dir / "assets/templates")
        self.env = Environment(loader=loader, autoescape=False)

        self.website_page_template = self.env.get_template("website_page.html")
        self.docs_page_template = self.env.get_template("docs_page.html")

        self.index_list_template = self.env.get_template("index_list.html")
        self.module_template = self.env.get_template("module.html")
        self.type_template = self.env.get_template("type.html")

        self.combined_pattern = self._build_combined_pattern()
        self.modules = modules
        self.guides = [p for p in pages if p.id != "indexpage"]

    def render_values(self, value, key=None):
        """Converts token text to HTML while preserving identifiers."""
        if isinstance(value, str):
            return value if key == "id" else self._convert_to_html(value)

        if isinstance(value, list):
            return [self.render_values(item) for item in value]

        if isinstance(value, dict):
            return {
                name: self.render_values(item, name)
                for name, item in value.items()
            }

        if is_dataclass(value) and not isinstance(value, type):
            rendered_fields = {
                field.name: self.render_values(
                    getattr(value, field.name),
                    field.name
                )
                for field in fields(value)
                if field.init
            }
            return type(value)(**rendered_fields)

        return value

    def render_module(self, module: Module) -> str:
        """Renders a module using the documentation page template."""
        content = self.module_template.render(module=module)

        return self.docs_page_template.render(
            title=module.name,
            guides=self.guides,
            modules=self.modules,
            content_title=module.name,
            content=content
        )

    def render_page(self, page: Page, is_docs_page: bool = True) -> str:
        """Renders a page using the website or docs page template."""
        if is_docs_page:
            return self.docs_page_template.render(
                title=page.name,
                guides=self.guides,
                modules=self.modules,
                content_title=page.name,
                content=page.description
            )

        else:
            return self.website_page_template.render(
                title=page.name,
                content=page.description
            )

    def render_index_list(self, items: list[Module | Page], name: str) -> str:
        """Renders a page using the index list template"""
        content = self.index_list_template.render(items=items)

        return self.docs_page_template.render(
            title=name,
            guides=self.guides,
            modules=self.modules,
            content_title=name,
            content=content
        )

    def render_type(self, data, page_title: str = "") -> str:
        """Renders a type using the documentation page template."""
        _data: Function | Struct | Typedef | ConstantGroup = data
        content = self.type_template.render(entry=_data)
        name = _data.name
        if page_title:
            name = page_title

        return self.docs_page_template.render(
            title=name,
            guides=self.guides,
            modules=self.modules,
            content_title=name,
            content=content
        )

    def _build_combined_pattern(self):
        """Builds one regex that can locate any token in a single scan."""
        alternatives = []

        for key, pattern in PATTERNS.items():
            # Each individual pattern uses named field groups. They become
            # ordinary groups here so alternatives can share field names.
            unnamed_pattern = re.sub(
                r"\(\?P<\w+>",
                "(",
                pattern.pattern,
            )
            alternatives.append(
                f"(?P<TOKEN_{key}>{unnamed_pattern})"
            )

        return re.compile("|".join(alternatives), re.DOTALL)

    def _replace_token(self, match: re.Match):
        key = match.lastgroup.removeprefix("TOKEN_")
        token_text = match.group(f"TOKEN_{key}")
        fields = PATTERNS[key].fullmatch(token_text).groupdict()

        # These fields are used in HTML attributes as well as text.
        # Decode the earlier text-context escaping, then escape for
        # attribute context. Text-context escaping is safe in both.
        if key == "ref":
            fields["target_id"] = html.escape(
                html.unescape(fields["target_id"]),
                quote=True,
            )

        elif key == "callout":
            fields["type"] = html.escape(
                html.unescape(fields["type"]),
                quote=True,
            )

            # get the par name from the content
            if fields["type"] == "par":
                parts = fields["content"].split("<", 1)
                fields["prefix"] = f"<strong>{parts[0]}</strong>"
                fields["content"] = f"<{parts[1]}"

            else:
                # make default types titlecase
                tmp: str = fields['type']
                type_str = tmp.capitalize()
                fields["prefix"] = f"<strong>{type_str}</strong>"

        elif key == "link":
            url = fields["url"]
            parts = fields["content"].split(":", 1)
            fields["url"] = f"{url}:{parts[0]}"
            fields["content"] = f"{parts[1]}"

        return HTML_TEMPLATES[key].format(**fields)

    def _convert_to_html(self, text: str) -> str:
        """Converts tokens into HTML, rendering nested tokens inside-out."""
        if not text:
            return ""

        rendered = html.escape(text, quote=False)
        while True:
            rendered, count = self.combined_pattern.subn(
                self._replace_token, rendered)
            if count == 0:
                break

        if "{{" in rendered:
            raise ValueError(
                f"unrendered token in: {rendered[:200]}"
            )

        return unesc(rendered)


def load_cache(filepath: Path):
    """Loads data objects from disk"""
    with open(filepath, "rb") as file:
        return pickle.load(file)


def build_search_index(guides: list[Page], modules: list[Module]) -> list[dict]:
    entries: list[dict] = []

    def _brief(obj) -> str:
        text = " ".join((getattr(obj, "brief", "") or "").split())
        return text[:BRIEF_LIMIT]

    def add(name, kind, module, url, brief=""):
        entries.append(
            {"n": name, "k": kind, "m": module, "u": url, "b": brief})

    for guide in guides:
        add(guide.name, "guide", "", f"{guide.id}.html", _brief(guide))

    for module in modules:
        add(module.name, "module", "", f"{module.id}.html", _brief(module))

        for func in module.functions:
            add(func.name, "function", module.name,
                f"{func.id}.html", _brief(func))

        for struct in module.structs:
            add(struct.name, "struct", module.name,
                f"{struct.id}.html", _brief(struct))

        for typedef in module.typedefs:
            add(typedef.name, "type", module.name,
                f"{typedef.id}.html", _brief(typedef))

        for constant in module.constants:
            add(constant.name, "constant", module.name,
                f"{module.id}__constants.html#{constant.id}")

        for group in module.constant_groups:
            add(group.name, "constants", module.name,
                f"{group.id}.html", _brief(group))

    return entries


def main():
    root_dir = Path(__file__).resolve().parents[1]
    website_dir = root_dir / "build/website"
    html_dir = website_dir / "html"
    html_dir.mkdir(parents=True, exist_ok=True)

    modules: list[Module] = load_cache(root_dir / "build/modules.pkl")
    pages: list[Page] = load_cache(root_dir / "build/pages.pkl")
    renderer = HTMLRenderer(root_dir, modules, pages)

    clean_modules: list[Module] = []
    clean_guides: list[Page] = []

    # generate module pages
    for module in modules:
        clean_module: Module = renderer.render_values(module)
        rendered = renderer.render_module(clean_module)
        clean_modules.append(clean_module)

        file = html_dir / f"{module.id}.html"
        file.write_text(rendered, encoding="utf-8")

        # generate function pages
        for func in clean_module.functions:
            rendered = renderer.render_type(func)
            file = html_dir / f"{func.id}.html"
            file.write_text(rendered, encoding="utf-8")

        # generate struct pages
        for struct in clean_module.structs:
            rendered = renderer.render_type(struct)
            file = html_dir / f"{struct.id}.html"
            file.write_text(rendered, encoding="utf-8")

        # generate typedef pages
        for typedef in clean_module.typedefs:
            rendered = renderer.render_type(typedef)
            file = html_dir / f"{typedef.id}.html"
            file.write_text(rendered, encoding="utf-8")

        # generate constant groups page
        for group in clean_module.constant_groups:
            rendered = renderer.render_type(group)
            file = html_dir / f"{group.id}.html"
            file.write_text(rendered, encoding="utf-8")

    # generate markdown pages
    for page in pages:
        clean_page: Page = renderer.render_values(page)
        rendered = renderer.render_page(clean_page)

        file = html_dir / f"{page.id}.html"
        if page.id == "indexpage":
            file = html_dir / "index.html"
        else:
            clean_guides.append(clean_page)

        file.write_text(rendered, encoding="utf-8")

    # generates website pages
    pages_dir = root_dir / "assets/pages"
    for f in pages_dir.iterdir():
        text = Path(f).read_text(encoding="utf-8")
        match = re.match(r"\s*<!--\stitle:\s*(.*?)\s*-->", text)
        page = Page("", match.group(1), "", text[match.end():])

        rendered = renderer.render_page(page, False)
        file = website_dir / f.name
        file.write_text(rendered, encoding="utf-8")

    # generate api_reference page
    rendered = renderer.render_index_list(clean_modules, "API Reference")
    file = html_dir / "api_reference.html"
    file.write_text(rendered, encoding="utf-8")

    # generate guides page
    rendered = renderer.render_index_list(clean_guides, "Guides")
    file = html_dir / "guides.html"
    file.write_text(rendered, encoding="utf-8")

    # build search data.
    variable = "window.PAL_SEARCH_INDEX"
    tmp = build_search_index(clean_guides, clean_modules)
    data = json.dumps(tmp, separators=(",", ":"))
    file = html_dir / "search-index.js"
    file.write_text(f"{variable}=" + data + ";\n", encoding="utf-8")

    print(f"html files generated successfully")


if __name__ == "__main__":
    main()
