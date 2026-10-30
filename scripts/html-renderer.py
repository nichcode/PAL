#!/usr/bin/env python3

import html
import pickle
import re
from dataclasses import is_dataclass, fields
from pathlib import Path

from jinja2 import Environment, FileSystemLoader
from models import TOKENS, Module, Page, token_pattern, unesc

# Token key -> HTML
HTML_TEMPLATES = {
    "code": "<code>{content}</code>",
    "bold": "<strong>{content}</strong>",
    "ref": '<a href="{target_id}.html">{content}</a>',
    "codeblock": "<pre><code>{content}</code></pre>",
    "callout": (
        '<div class="callout {type}">'
        '<strong class="callout-title">{type}</strong>{content}</div>'
    ),
    "table": "<table>{content}</table>",
    "row": "<tr>{content}</tr>",
    "cell": "<td>{content}</td>",
    "para": '<div class="para">{content}</div>'
}

assert HTML_TEMPLATES.keys() == TOKENS.keys(), (
    "renderer is missing a token"
)

PATTERNS = {
    key: token_pattern(key)
    for key in TOKENS
}


class HTMLRenderer:
    def __init__(self, root_dir):
        self.root_dir = root_dir
        loader = FileSystemLoader(root_dir / "assets/templates")
        self.env = Environment(loader=loader, autoescape=False)

        self.website_page_template = self.env.get_template(
            "website_page.html"
        )

        self.docs_page_template = self.env.get_template(
            "docs_page.html"
        )

        self.modules_list = ""
        self.guides_list = ""
        self.combined_pattern = self._build_combined_pattern()

    def render_module(self, module: Module) -> str:
        """Renders a module using the documentation page template."""
        rendered_module: Module = self._render_values(module)
        page_type = "intro"

        return self.docs_page_template.render(
            title_page_type=page_type,
            title=rendered_module.name,
            guides=self.guides_list,
            modules=self.modules_list,
            page_content_title=rendered_module.name,
            page_type=page_type,
            content=module.description,
            toc_page_type=page_type,
            toc=""
        )

    def render_page(self, page: Page, is_docs_page: bool = True) -> str:
        """Renders a page using the website or docs page template."""
        if is_docs_page:
            rendered_page: Page = self._render_values(page)
            page_type = "intro"

            return self.docs_page_template.render(
                title_page_type=page_type,
                title=rendered_page.title,
                guides=self.guides_list,
                modules=self.modules_list,
                page_content_title=rendered_page.title,
                page_type=page_type,
                content=rendered_page.description,
                toc_page_type=page_type,
                toc=""
            )
        else:
            return self.website_page_template.render(
                title=page.title,
                content=page.description
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

        # Escape raw text first; token syntax survives HTML escaping.
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

    def _render_values(self, value, key=None):
        """Converts model text to HTML while preserving identifiers."""
        if isinstance(value, str):
            return value if key == "id" else self._convert_to_html(value)

        if isinstance(value, list):
            return [self._render_values(item) for item in value]

        if isinstance(value, dict):
            return {
                name: self._render_values(item, name)
                for name, item in value.items()
            }

        if is_dataclass(value) and not isinstance(value, type):
            rendered_fields = {
                field.name: self._render_values(
                    getattr(value, field.name),
                    field.name,
                )
                for field in fields(value)
                if field.init
            }
            return type(value)(**rendered_fields)

        return value


def load_cache(filepath: Path):
    """Loads data objects from disk"""
    with open(filepath, "rb") as file:
        return pickle.load(file)


def main():
    root_dir = Path(__file__).resolve().parents[1]
    website_dir = root_dir / "build/website"
    html_dir = website_dir / "html"
    html_dir.mkdir(parents=True, exist_ok=True)

    modules: list[Module] = load_cache(root_dir / "build/modules.pkl")
    pages: list[Page] = load_cache(root_dir / "build/pages.pkl")
    renderer = HTMLRenderer(root_dir)

    for module in modules:
        rendered = renderer.render_module(module)
        file = html_dir / f"{module.id}.html"
        file.write_text(rendered, encoding="utf-8")

    for page in pages:
        rendered = renderer.render_page(page)
        file = html_dir / f"{page.id}.html"
        if page.id == "indexpage":
            file = html_dir / "index.html"

        file.write_text(rendered, encoding="utf-8")

    # generates page from pages directory
    pages_dir = root_dir / "assets/pages"
    for f in pages_dir.iterdir():
        text = Path(f).read_text(encoding="utf-8")
        match = re.match(r"\s*<!--\stitle:\s*(.*?)\s*-->", text)
        page = Page("", match.group(1), "", text[match.end():])

        rendered = renderer.render_page(page, False)
        file = website_dir / f.name
        file.write_text(rendered, encoding="utf-8")

    print(f"html files generated successfully")


if __name__ == "__main__":
    main()
