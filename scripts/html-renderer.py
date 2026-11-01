#!/usr/bin/env python3

import html
import pickle
import re
from dataclasses import is_dataclass, fields, replace
from pathlib import Path

from jinja2 import Environment, FileSystemLoader
from models import (
    Constant,
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
    "callout": '<div class="callout" type="{type}"><strong>{type}</strong>{content}</div>',
    "table": "<table>{content}</table>",
    "row": "<tr>{content}</tr>",
    "cell": "<td>{content}</td>",
    "para": "<p>{content}</p>"
}

assert HTML_TEMPLATES.keys() == TOKENS.keys(), (
    "renderer is missing a token"
)

PATTERNS = {
    key: token_pattern(key)
    for key in TOKENS
}


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
        desc = module.brief + module.description

        content = self.module_template.render(
            module=module,
            desc=desc
        )

        return self.docs_page_template.render(
            title=module.name,
            guides=self.guides,
            modules=self.modules,
            page_type="intro",
            content_title=module.name,
            content=content,
            toc=""
        )

    def render_page(self, page: Page, is_docs_page: bool = True) -> str:
        """Renders a page using the website or docs page template."""
        if is_docs_page:
            return self.docs_page_template.render(
                title=page.name,
                guides=self.guides,
                modules=self.modules,
                page_type="intro",
                content_title=page.name,
                content=page.description,
                toc=""
            )

        else:
            return self.website_page_template.render(
                title=page.name,
                content=page.description
            )

    def render_index_list(self, items) -> str:
        """Renders a page using the index list template"""
        return self.index_list_template.render(items=items)

    # def render_function(self, function: Function) -> str:
    #     """Renders a module using the documentation page template."""
    #     rendered_func: Function = self.render_values(function)

    #     print(rendered_func.description)

    #     return ""

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

        return re.compile("|".join(alternatives), re.S)

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


def write_index_html_page(renderer: HTMLRenderer, html_dir: Path, items: list[Module | Page], name: str):
    """Writes an html file using index list and docs templates"""
    tmp_items: list[Module | Page] = []

    for item in items:
        if name == "Guides":
            copied_item = replace(item, description=item.brief)
        else:
            copied_item = replace(item, description=item.brief + item.description)
        tmp_items.append(copied_item)

    rendered_index_list = renderer.render_index_list(tmp_items)
    page = Page("", name, "", rendered_index_list)

    rendered = renderer.render_page(page)
    file = html_dir / f"{name.lower()}.html"
    file.write_text(rendered, encoding="utf-8")


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
        # for func in clean_module.functions:
        #     rendered = renderer.render_function(func)

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

    write_index_html_page(renderer, html_dir, clean_modules, "API_Reference")
    write_index_html_page(renderer, html_dir, clean_guides, "Guides")

    print(f"html files generated successfully")


if __name__ == "__main__":
    main()
