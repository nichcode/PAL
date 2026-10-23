#!/usr/bin/env python3

import sys
import os
import subprocess
from pathlib import Path

import shutil
import re
from jinja2 import Environment, FileSystemLoader
from xml.etree import ElementTree as ET

import html

ROOT_DIR = Path(__file__).resolve().parents[1]
ASSETS_DIR = ROOT_DIR / "assets"
DOCS_DIR = ROOT_DIR / "docs"
BUILD_DIR = ROOT_DIR / "build"
OUTPUT_DIR = BUILD_DIR / "website"
HTML_DIR = OUTPUT_DIR / "html"
XML_DIR = BUILD_DIR / "xml"

env = Environment(loader=FileSystemLoader(ASSETS_DIR / "templates"))
website_page_template = env.get_template("website_page.html")
docs_page_template = env.get_template("docs_page.html")

GUIDES = [] # static array of guides for all docs page
MODULES = [] # static array of modules for all docs page

def build_module_page(module):
    content = []
    content.append(f"<h1>{html.escape(module['title'])}</h1>")

    titles = []
    heads = []
    kinds = []
    brief = "Tmp brief"

    if module['functions']:
        titles.append("<h3>Functions</h3>")
        heads.append("Function")
        kinds.append("functions")

    if module['structs']:
        titles.append("<h3>Structs</h3>")
        heads.append("Struct")
        kinds.append("structs")

    if module['types']:
        titles.append("<h3>Types</h3>")
        heads.append("Type")
        kinds.append("types")

    if module['constants'] or module['subgroups']:
        titles.append("<h3>Constants</h3>")
        heads.append("Constant")
        kinds.append("constants")

    for index, head in enumerate(heads):
        content.append(titles[index])
        content.append("<table>")
        content.append("<tr>")
        content.append(f"<th>{head}</th>")
        content.append("<th>Brief</th>")
        content.append("</tr>")

        for refid, name in module[kinds[index]]:
            content.append("<tr>")
            content.append(f'<td><a href="{refid}.html">{name}</a></td>')
            content.append(f'<td>{brief}</td>')
            content.append("</tr>")

        content.append("</table>")

    return {"title": module["title"], "content": "\n".join(content)}

def build_modules_list(modules):
    for refid in modules:
        module = get_module(refid)

        kinds = []
        links = []
        kinds.append("none")
        links.append(f'<a href="{module['refid']}.html">{module['title']}</a>')

        if module["functions"]:
            kinds.append("functions")
            links.append(f'<a href="{module['refid']}__functions.html">Functions</a>')

        if module["structs"]:
            kinds.append("structs")
            links.append(f'<a href="{module['refid']}__structs.html">Structs</a>')

        if module["types"]:
            kinds.append("types")
            links.append(f'<a href="{module['refid']}__types.html">Types</a>')

        if module["constants"] or module["subgroups"]:
            kinds.append("constants")
            links.append(f'<a href="{module['refid']}__constants.html">Constants</a>')

        for index, kind in enumerate(kinds):
            MODULES.append('<li class="sidebar-group">')
            MODULES.append('<div class="sidebar-heading">')
            MODULES.append('<button class="sidebar-arrow" aria-expanded="false"></button>')
            MODULES.append(links[index])
            MODULES.append('</div>')
            MODULES.append('<ul class="sidebar-tree" hidden>')

            if kind == "none":
                continue

            for refid, name in module[kind]:
                MODULES.append(f'<li><a href="{refid}.html">{name}</a></li>')

            if kind == "constants":
                for refid, name in module["subgroups"]:
                    MODULES.append(f'<li><a href="{refid}.html">{name}</a></li>')

            MODULES.append('</ul>')
            MODULES.append('</li>')

    MODULES.append('</li>')

def main():
    os.chdir(DOCS_DIR) # change working directory so doxygen works as intended
    HTML_DIR.mkdir(parents=True, exist_ok=True)
    generate_doxygen()

    md_pages = find_md_pages()
    modules = find_modules()
    copy_assets()

    build_guides_list(md_pages)
    build_modules_list(modules)

    for refid in modules:
        module = get_module(refid)
        write_docs_page(HTML_DIR / f"{module['refid']}.html", build_module_page(module))

    return 0

if __name__ == "__main__":
    main()