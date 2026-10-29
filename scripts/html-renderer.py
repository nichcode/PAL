#!/usr/bin/env python3

import re
from pathlib import Path
from jinja2 import Environment, FileSystemLoader
from models import *


@dataclass
class HtmlPage:
    title: str
    body: str
    page_type: str
    filename: str
    guides_sidebar: str
    modules_sidebar: str
    toc_sidebar: str
    is_website: bool


class HTMLRenderer:
    def __init__(self, root_dir, modules, pages):
        self.root_dir = root_dir
        self.modules: list[Module] = modules
        self.pages: list[Page] = pages

        self.env = Environment(loader=FileSystemLoader(
            root_dir / "assets/templates"))
        self.website_page_template = self.env.get_template("website_page.html")
        self.docs_page_template = self.env.get_template("docs_page.html")

        self.modules_sidebar = self._build_modules_sidebar()
        self.guides_sidebar = self._build_guides_sidebar()
        self.toc_sidebar = self._build_toc_sidebar()

    def generate(self):
        """Generates html pages"""

        self._generate_website_pages()
        self._generate_pages()
        self._generate_guides_page()
        self._generate_api_reference_page()

    def _write_page(self, page: HtmlPage):
        """Generates html page"""

        if page.is_website:
            output_dir = self.root_dir / "build/website"

            html_page = self.website_page_template.render(
                title=page.title,
                content=page.body)

        else:
            output_dir = self.root_dir / "build/website/html"

            html_page = self.docs_page_template.render(
                title_page_type=page.page_type,
                title=page.title,
                guides=page.guides_sidebar,
                modules=page.modules_sidebar,
                page_content_title=page.title,
                page_type=page.page_type,
                content=page.body,
                toc_page_type=page.page_type,
                toc=page.toc_sidebar)

        output_dir.mkdir(parents=True, exist_ok=True)
        output_file = output_dir / page.filename
        output_file.write_text(html_page, encoding="utf-8")

    def _generate_website_pages(self):
        """Generates html pages from assets/pages directory"""

        pages_dir = self.root_dir / "assets/pages"
        for file in pages_dir.iterdir():
            text = Path(file).read_text(encoding="utf-8")
            match = re.match(r"\s*<!--\stitle:\s*(.*?)\s*-->", text)

            html_page = HtmlPage(
                match.group(1),
                text[match.end():],
                "",
                file.name,
                "",
                "",
                "",
                True)

            self._write_page(html_page)

    def _generate_pages(self):
        """Generates markdown pages"""

        for page in self.pages:
            filename = f"{page.refid}.html"
            page_type = "default"
            toc_sidebar = self.toc_sidebar

            if page.refid == "indexpage":
                filename = "index.html"
                toc_sidebar = ""
                page_type = "intro"

            html_page = HtmlPage(
                page.name,
                page.detailed,
                page_type,
                filename,
                self.guides_sidebar,
                self.modules_sidebar,
                toc_sidebar,
                False)

            self._write_page(html_page)

    def _generate_guides_page(self):
        """Generates guides.html"""

        body = [
            '<table class="guides-table">',
            '<thead>',
            '<tr class="guides-table-row">',
            '<th class="guides-table-header">Guide</th>',
            '<th class="guides-table-header">Description</th>',
            '</tr>',
            '</thead>']

        for page in self.pages:
            if page.refid in ["indexpage"]:
                continue

            body.extend([
                '<tr class="guides-table-row">',
                f'<td class="guides-table-data"><a href="{page.refid}.html">{page.name}</a></td>',
                f'<td class="guides-table-data">{page.brief}</td>',
                '</tr>'
            ])

        body.append('</table>')

        html_page = HtmlPage(
            "Guides",
            "\n".join(body),
            "intro",
            "guides.html",
            self.guides_sidebar,
            self.modules_sidebar,
            "",
            False)

        self._write_page(html_page)

    def _generate_api_reference_page(self):
        """Generates api_reference.html"""

        body = ['<ul class="api-list">']
        for module in self.modules:
            body.extend([
                '<li class="api-list-item">',
                f'<a href="{module.refid}.html">{module.name}</a>',
                f'<p class="api-list-item-desc">{module.detailed}</p>',
                '</li>'
            ])

        body.append('</ul>')

        html_page = HtmlPage(
            "API Reference",
            "\n".join(body),
            "intro",
            "api_reference.html",
            self.guides_sidebar,
            self.modules_sidebar,
            self.toc_sidebar,
            False)

        self._write_page(html_page)

    def _build_modules_sidebar(self):
        """Build modules sidebar"""

        body = []
        for module in self.modules:
            body.extend([
                '<li class="sidebar-group">',
                '<div class="sidebar-heading">',
                '<button class="sidebar-arrow" aria-expanded="false"></button>',
                f'<a href="{module.refid}.html">{module.name}</a>',
                '</div>',
                '<ul class="sidebar-tree" hidden>'])

            for func in module.functions:
                body.append(
                    f'<li><a href="f{func.refid}.html">{func.name}</a></li>')

            for struct in module.structs:
                body.append(
                    f'<li><a href="{struct.refid}.html">{struct.name}</a></li>')

            for typedef in module.typedefs:
                body.append(
                    f'<li><a href="{typedef.refid}.html">{typedef.name}</a></li>')

            for constant in module.constants:
                link = f"{module.refid}__constants.html"
                body.append(
                    f'<li><a href="{link}#{constant.refid}">{constant.name}</a></li>')

            for group in module.constant_groups:
                body.append(
                    f'<li><a href="{group.refid}.html">{group.name}</a></li>')

            body.extend([
                '</ul>',
                '</li>'])

        return "\n".join(body)

    def _build_guides_sidebar(self):
        """Build guides sidebar"""

        body = []
        for page in self.pages:
            if page.refid in ["indexpage"]:
                continue

            body.append(
                f'<li><a href="{page.refid}.html">{page.name}</a></li>')

        return "\n".join(body)

    def _build_toc_sidebar(self):
        """Build toc sidebar"""

        return ""


def main():
    root_dir = Path(__file__).resolve().parents[1]
    modules = load_cache(root_dir / "build/modules.pkl")
    pages = load_cache(root_dir / "build/pages.pkl")

    renderer = HTMLRenderer(root_dir, modules, pages)
    renderer.generate()

    print(f"html files generated successfully")


if __name__ == "__main__":
    main()
