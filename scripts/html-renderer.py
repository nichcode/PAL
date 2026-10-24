#!/usr/bin/env python3

from pathlib import Path
import pickle
import sys
import shutil

import re
from jinja2 import Environment, FileSystemLoader
from models import *


class HTMLRenderer:
    def __init__(self, root_dir, modules, pages) -> None:
        self.root_dir = root_dir
        self.modules: list[Module] = modules
        self.pages: list[Page] = pages

        self.env = Environment(loader=FileSystemLoader(root_dir / "assets/templates"))
        self.website_page_template = self.env.get_template("website_page.html")
        self.docs_page_template = self.env.get_template("docs_page.html")

        self.modules_list = self._build_modules_list()
        self.guides_list = self._build_guides_list()

    def copy_assets(self):
        """Copies assets into output HTML directory"""
        assets_dir = self.root_dir / "assets"
        output_dir = self.root_dir / "build/website/html"

        if output_dir.exists():
            shutil.rmtree(output_dir)

        output_dir.mkdir(parents=True)
        for item in assets_dir.iterdir():
            if item.name == "templates":
                continue
            elif item.name == "pages":
                continue

            for file in item.iterdir():
                shutil.copy(file, output_dir / file.name)

        print(f"assets copied successfully")

    def generate_website_pages(self):
        """Generates pages with sources from assets/pages directory"""
        pages_dir = self.root_dir / "assets/pages"
        output_dir = self.root_dir / "build/website"

        for file in pages_dir.iterdir():
            text = Path(file).read_text(encoding="utf-8")

            match = re.match(r"\s*<!--\stitle:\s*(.*?)\s*-->", text)
            if match:
                title = match.group(1)
                body = text[match.end():]

            output_file = output_dir / file.name

            html_page = self.website_page_template.render(title=title, content=body)
            output_file.write_text(html_page, encoding="utf-8")

        print("website pages generated successfully")

    def generate_pages(self):
        """Generates markdown pages"""
        output_dir = self.root_dir / "build/website/html"

        for page in self.pages:
            output_file = output_dir / f"{page.refid}.html"
            if page.refid == "indexpage":
                output_file = output_dir / "index.html"

            html_page = self.docs_page_template.render(
                title=page.name,
                page_content_title=page.name,
                content=page.body)

            html_page = html_page.replace("<!-- guides -->", self.guides_list)
            html_page = html_page.replace("<!-- modules -->", self.modules_list)
            output_file.write_text(html_page, encoding="utf-8")

        print("markdown pages generated successfully")

    def generate_guides_page(self):
        """Generates guides.html"""
        output_dir = self.root_dir / "build/website/html"
        name = "Guides"

        content = ['<ul class="list">']
        for page in self.pages:
            if page.refid in ["indexpage"]:
                continue

            content.extend([
                '<li class="list-item">',
                f'<a href="{page.refid}.html">{page.name}</a>',
                f'<p class="list-item-desc">{page.desc}</p>',
                '</li>'
            ])
        
        content.append('</ul>')
        
        html_page = self.docs_page_template.render(
            title=name,
            page_content_title=name,
            content="\n".join(content))

        html_page = html_page.replace("<!-- guides -->", self.guides_list)
        html_page = html_page.replace("<!-- modules -->", self.modules_list)

        output_file = output_dir / "guides.html"
        output_file.write_text(html_page, encoding="utf-8")

        print("guides.html generated successfully")

    def generate_api_reference_page(self):
        """Generates api_reference.html"""
        output_dir = self.root_dir / "build/website/html"
        name = "API Reference"

        content = ['<ul class="list">']
        for module in self.modules:
            content.extend([
                '<li class="list-item">',
                f'<a href="{module.refid}.html">{module.name}</a>',
                f'<p class="list-item-desc">{module.desc}</p>',
                '</li>'
            ])
        
        content.append('</ul>')
        
        html_page = self.docs_page_template.render(
            title=name,
            page_content_title=name,
            content="\n".join(content))

        html_page = html_page.replace("<!-- guides -->", self.guides_list)
        html_page = html_page.replace("<!-- modules -->", self.modules_list)

        output_file = output_dir / "api_reference.html"
        output_file.write_text(html_page, encoding="utf-8")

        print("api_reference.html generated successfully")

    def generate_module_pages(self):
        """Generates base and subsequent pages for modules"""
        output_dir = self.root_dir / "build/website/html"
        filename = "pal_system.h"

        for module in self.modules:
            content = [
                f'<code>#include &lt;{filename}&gt;</code>',
                f'<p class="module-desc">{module.desc}</p>',
                '<h2 class="module-section-title">Reference</h2>', 
                '<ul class="module-section-list">'
            ]

            if module.functions:
                content.extend([
                    '<li>',
                    f'<a href="{module.refid}__functions.html">Functions</a>',
                    '</li>'
                ])

            if module.structs:
                content.extend([
                    '<li>',
                    f'<a href="{module.refid}__structs.html">Structs</a>',
                    '</li>'
                ])

            if module.typedefs:
                content.extend([
                    '<li>',
                    f'<a href="{module.refid}__typedefs.html">Typedefs</a>',
                    '</li>'
                ])

            if module.constants or module.constant_groups:
                content.extend([
                    '<li>',
                    f'<a href="{module.refid}__constants.html">Constants</a>',
                    '</li>'
                ])

            content.append('</ul>')

            content.extend([
                '<footer class="module-license">',
                f'{module.license}',
                '</footer>'
            ])
            
            html_page = self.docs_page_template.render(
                title=module.name,
                page_content_title=module.name,
                content="\n".join(content))
    
            html_page = html_page.replace("<!-- guides -->", self.guides_list)
            html_page = html_page.replace("<!-- modules -->", self.modules_list)
    
            output_file = output_dir / f"{module.refid}.html"
            output_file.write_text(html_page, encoding="utf-8")

        print("module pages generated successfully")

    def _build_modules_list(self):
        """Build modules sidebar list"""
        modules = []

        for module in self.modules:
            modules.extend([
                '<li class="sidebar-group">',
                '<div class="sidebar-heading">',
                '<button class="sidebar-arrow" aria-expanded="false"></button>',
                f'<a href="{module.refid}.html">{module.name}</a>',
                '</div>',
                '<ul class="sidebar-tree" hidden>'
            ])

            if module.functions:
                modules.append(f'<li><a href="{module.refid}__functions.html">Functions</a></li>')

            if module.structs:
                modules.append(f'<li><a href="{module.refid}__structs.html">Structs</a></li>')

            if module.typedefs:
                modules.append(f'<li><a href="{module.refid}__typedefs.html">Typedefs</a></li>')

            if module.constants or module.constant_groups:
                modules.append(f'<li><a href="{module.refid}__constants.html">Constants</a></li>')

            modules.extend([
                '</ul>',
                '</li>'
            ])

        return "\n".join(modules)
        
    def _build_guides_list(self):
        """Build guides sidebar list"""
        guides = []

        # make getting started always first
        self.pages.sort(key=lambda page: (page.refid != "getting_started", page.refid))
        for page in self.pages:
            if page.refid in ["indexpage"]:
                continue

            guides.append(f'<li><a href="{page.refid}.html">{page.name}</a></li>')

        return "\n".join(guides)


def main():
    root_dir = Path(__file__).resolve().parents[1]

    filepath = root_dir / "build/modules.pkl"
    try:
        with open(filepath, "rb") as file:
            modules: list[Module] = pickle.load(file)

    except (pickle.PickleError, EOFError, AttributeError) as e:
        print(f"modules cache is corrupted or out of date: {e}.")
        sys.exit(1)

    filepath = root_dir / "build/pages.pkl"
    try:
        with open(filepath, "rb") as file:
            pages: list[Page] = pickle.load(file)

    except (pickle.PickleError, EOFError, AttributeError) as e:
        print(f"pages cache is corrupted or out of date: {e}.")
        sys.exit(1)

    renderer = HTMLRenderer(root_dir, modules, pages)
    renderer.copy_assets()
    renderer.generate_website_pages()
    renderer.generate_pages()
    renderer.generate_guides_page()
    renderer.generate_api_reference_page()
    renderer.generate_module_pages()

if __name__ == "__main__":
    main()