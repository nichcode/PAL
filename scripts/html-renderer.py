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
    guides_list: str
    modules_list: str
    toc_list: str
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

        # self.modules_list = self._build_modules_list()
        # self.guides_list = self._build_guides_list()
        # self.toc_list = self._build_toc_list()

    def generate(self):
        """Generates html pages"""

        self._generate_website_pages()

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
                guides=page.guides_list,
                modules=page.modules_list,
                page_content_title=page.title,
                page_type=page.page_type,
                content=page.body,
                toc_page_type=page.page_type,
                toc=page.toc_list)

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


def main():
    root_dir = Path(__file__).resolve().parents[1]
    modules = load_cache(root_dir / "build/modules.pkl")
    pages = load_cache(root_dir / "build/pages.pkl")

    renderer = HTMLRenderer(root_dir, modules, pages)
    renderer.generate()

    print(f"html files generated successfully")


if __name__ == "__main__":
    main()
