#!/usr/bin/env python3

import re
import pickle
from pathlib import Path
from jinja2 import Environment, FileSystemLoader
from models import *


class HTMLRenderer:
    def __init__(self, root_dir):
        self.root_dir = root_dir
        loader = FileSystemLoader(root_dir / "assets/templates")
        env = Environment(loader=loader)

        self.website_page_template = env.get_template("website_page.html")
        self.docs_page_template = env.get_template("docs_page.html")
        self.patterns: Dict[str, re.Pattern] = {}

        # compile expressions from TOKENS
        for token in TOKENS:
            if token == "ref":
                self.patterns[token] = re.compile(r"\{\{ref:(.*?):(.*?)\}\}")
            elif token == "callout":
                self.patterns[token] = re.compile(
                    r"\{\{callout:(.*?):(.*?)\}\}")
            else:
                self.patterns[token] = re.compile(
                    rf"\{{{{ {token}:(.*?)\}}}}", re.DOTALL)

    def repl_ref(self, match: re.Match):
        """Cross reference links"""
        target_id = match.group(1)
        content = match.group(2)
        return f'<a href="{target_id}.html">{content}</a>'

    def repl_callout(self, match: re.Match):
        """Notes, warnings, ...)"""
        callout_type = match.group(1)
        content = match.group(2)
        return f'<div class="callout {callout_type}"><strong>{callout_type.capitalize()}</strong>{content}</div>'

    def convert_to_html(self, text: str) -> str:
        """Converts tokens into html tags"""
        if not text:
            return ""

        rendered = text
        tag_map = {
            "code": ("<code>", "</code>"),
            "strong": ("<strong>", "</strong>"),
            "codeblock": ("<pre><code>", "</code></pre>"),
            "table": ("<table>", "</table>"),
            "row": ("<tr>", "</tr>"),
            "cell": ("<td>", "</td>"),
        }

        if "ref" in self.patterns:
            rendered = self.patterns["ref"].sub(self.repl_ref, rendered)

        if "callout" in self.patterns:
            rendered = self.patterns["callout"].sub(
                self.repl_callout, rendered)

        for name, pattern in self.patterns.items():
            if name in ("ref", "callout"):
                continue

            if name in tag_map:
                open_tag, close_tag = tag_map[name]
                rendered = pattern.sub(rf"{open_tag}\1{close_tag}", rendered)

        return rendered

    def render_module(self, module: Module) -> str:
        """Renders a module data object"""
        title = self.convert_to_html(module.title)
        brief = self.convert_to_html(module.brief)
        description = self.convert_to_html(module.description)

        return ""


def load_cache(filepath: Path):
    """Loads data objects from disk"""
    with open(filepath, "rb") as file:
        return pickle.load(file)


def main():
    root_dir = Path(__file__).resolve().parents[1]
    modules = load_cache(root_dir / "build/modules.pkl")
    pages = load_cache(root_dir / "build/pages.pkl")
    renderer = HTMLRenderer(root_dir)

    for module in modules:
        renderer.render_module(module)

    print(f"html files generated successfully")


if __name__ == "__main__":
    main()
