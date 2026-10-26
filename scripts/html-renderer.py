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
        self.output_dir = root_dir / "build/website/html"
        self.modules: list[Module] = modules
        self.pages: list[Page] = pages

        self.env = Environment(loader=FileSystemLoader(root_dir / "assets/templates"))
        self.website_page_template = self.env.get_template("website_page.html")
        self.docs_page_template = self.env.get_template("docs_page.html")

        self.modules_list = self._build_modules_list()
        self.guides_list = self._build_guides_list()
        self.toc_list = self._build_toc_list()

    def copy_assets(self):
        """Copies assets into output HTML directory"""
        assets_dir = self.root_dir / "assets"

        if self.output_dir.exists():
            shutil.rmtree(self.output_dir)

        self.output_dir.mkdir(parents=True)
        for item in assets_dir.iterdir():
            if item.name == "templates":
                continue
            elif item.name == "pages":
                continue

            for file in item.iterdir():
                shutil.copy(file, self.output_dir / file.name)

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

        for page in self.pages:
            output_file = self.output_dir / f"{page.refid}.html"
            toc = self.toc_list
            page_type = "default"

            if page.refid == "indexpage":
                output_file = self.output_dir / "index.html"
                toc = ""
                page_type = "intro"

            html_page = self.docs_page_template.render(
                title_page_type=page_type,
                title=page.name,
                guides=self.guides_list,
                modules=self.modules_list,
                page_content_title=page.name,
                page_type=page_type,
                content=page.body,
                toc_page_type=page_type,
                toc=toc)

            output_file.write_text(html_page, encoding="utf-8")

        print("markdown pages generated successfully")

    def generate_guides_page(self):
        """Generates guides.html"""
        name = "Guides"
        page_type = "intro"

        content = [
            '<table class="guides-table">',
            '<thead>',
            '<tr class="guides-table-row">',
            '<th class="guides-table-header">Guide</th>',
            '<th class="guides-table-header">Description</th>',
            '</tr>',
            '</thead>'
        ]

        for page in self.pages:
            if page.refid in ["indexpage"]:
                continue

            content.extend([
                '<tr class="guides-table-row">',
                f'<td class="guides-table-data"><a href="{page.refid}.html">{page.name}</a></td>',
                f'<td class="guides-table-data">{page.desc}</td>',
                '</tr>'
            ])
        
        content.append('</table>')
        
        html_page = self.docs_page_template.render(
            title_page_type=page_type,
            title=name,
            guides=self.guides_list,
            modules=self.modules_list,
            page_content_title=name,
            page_type=page_type,
            content="\n".join(content),
            toc_page_type=page_type,
            toc=self.toc_list)

        output_file = self.output_dir / "guides.html"
        output_file.write_text(html_page, encoding="utf-8")

        print("guides.html generated successfully")

    def generate_api_reference_page(self):
        """Generates api_reference.html"""
        name = "API Reference"
        page_type = "intro"

        content = ['<ul class="api-list">']
        for module in self.modules:
            content.extend([
                '<li class="api-list-item">',
                f'<a href="{module.refid}.html">{module.name}</a>',
                f'<p class="api-list-item-desc">{module.desc}</p>',
                '</li>'
            ])
        
        content.append('</ul>')
        
        html_page = self.docs_page_template.render(
            title_page_type=page_type,
            title=name,
            guides=self.guides_list,
            modules=self.modules_list,
            page_content_title=name,
            page_type=page_type,
            content="\n".join(content),
            toc_page_type=page_type,
            toc=self.toc_list)

        output_file = self.output_dir / "api_reference.html"
        output_file.write_text(html_page, encoding="utf-8")

        print("api_reference.html generated successfully")

    def generate_module_pages(self):
        """Generates base and subsequent pages for modules"""
        page_type = "default"

        for module in self.modules:
            content = [
                f'<code>#include &lt;{module.file}&gt;</code>',
                f'<p class="module-desc">{module.desc}</p>'
            ]

            if module.functions:
                content.extend([
                    '<p class="module-sub-title">Functions</p>',
                    f'<ul class="module-sub-list">'
                ])

                for func in module.functions:
                    content.extend([
                        '<li class="module-sub-list-item">',
                        f'<a href="function__{func.name}.html">{func.name}</a>',
                        f'<p class="module-sub-list-item-desc">{func.brief}</p>',
                        '</li>'
                    ])

                content.append('</ul>')

            if module.structs:
                content.extend([
                    '<p class="module-sub-title">Structs</p>',
                    f'<ul class="module-sub-list">'
                ])

                for struct in module.structs:
                    content.extend([
                        '<li class="module-sub-list-item">',
                        f'<a href="struct__{struct.name}.html">{struct.name}</a>',
                        f'<p class="module-sub-list-item-desc">{struct.brief}</p>',
                        '</li>'
                    ])

                content.append('</ul>')

            if module.typedefs:
                content.extend([
                    '<p class="module-sub-title">Typedefs</p>',
                    f'<ul class="module-sub-list">'
                ])

                for typedef in module.typedefs:
                    content.extend([
                        '<li class="module-sub-list-item">',
                        f'<a href="typedef__{typedef.name}.html">{typedef.name}</a>',
                        f'<p class="module-sub-list-item-desc">{typedef.brief}</p>',
                        '</li>'
                    ])

                content.append('</ul>')

            if module.functions or module.constant_groups:
                content.extend([
                    '<p class="module-sub-title">Constants</p>',
                    f'<ul class="module-sub-list">'
                ])

                for constant in module.constants:
                    link = f"{module.refid}__constants.html"
                    content.extend([
                        '<li class="module-sub-list-item">',
                        f'<a href="{link}#{constant.refid}">{constant.name}</a>',
                        f'<p class="module-sub-list-item-desc">{constant.desc}</p>',
                        '</li>'
                    ])

                for group in module.constant_groups:
                    content.extend([
                        '<li class="module-sub-list-item">',
                        f'<a href="constant_group__{group.refid}.html">{group.name}</a>'
                        '</li>'
                    ])

                content.append('</ul>')

            content.extend([
                '<footer class="module-license">',
                f'{module.license}',
                '</footer>'
            ])
            
            html_page = self.docs_page_template.render(
                title_page_type=page_type,
                title=module.name,
                guides=self.guides_list,
                modules=self.modules_list,
                page_content_title=module.name,
                page_type=page_type,
                content="\n".join(content),
                toc_page_type=page_type,
                toc=self.toc_list)
    
            output_file = self.output_dir / f"{module.refid}.html"
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

            for func in module.functions:
                modules.append(f'<li><a href="function__{func.name}.html">{func.name}</a></li>')

            for struct in module.structs:
                modules.append(f'<li><a href="struct__{struct.name}.html">{struct.name}</a></li>')

            for typedef in module.typedefs:
                modules.append(f'<li><a href="typedef__{typedef.name}.html">{typedef.name}</a></li>')

            for constant in module.constants:
                link = f"{module.refid}__constants.html"
                modules.append(f'<li><a href="{link}#{constant.refid}">{constant.name}</a></li>')

            for group in module.constant_groups:
                modules.append(f'<li><a href="constant_group__{group.refid}.html">{group.name}</a></li>')

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

    def _build_toc_list(self):
        """Build toc list"""
        # toc = ['<h3 class="toc-title">On This Page</h3>']
        toc = []

        return "\n".join(toc)

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