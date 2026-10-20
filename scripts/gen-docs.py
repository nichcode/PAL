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

def read_html_page(filepath):
    text = Path(filepath).read_text(encoding="utf-8")

    # check for the tile of the page
    # this must be on the first line and in the format <!-- title: site title -->
    match_obj = re.match(r"\s*<!--\stitle:\s*(.*?)\s*-->", text)
    if match_obj:
        title = match_obj.group(1)
        content = text[match_obj.end():]
    else:
        # use the filename as the title
        title = Path(filepath).stem
        content = text

    return {"title": title, "content": content}

def write_website_page(filename, page):
    file = OUTPUT_DIR / filename
    html_page = website_page_template.render(
        title=page["title"],
        content=page["content"]
    )

    file.write_text(html_page, encoding="utf-8")

def write_docs_page(filename, page):
    file = HTML_DIR / filename
    html_page = docs_page_template.render(
        title=page["title"],
        content=page["content"],
    )

    html_page = html_page.replace("<!-- guides -->", "\n".join(GUIDES))
    html_page = html_page.replace("<!-- modules -->", "\n".join(MODULES))
    file.write_text(html_page, encoding="utf-8")

def copy_assets():
    if HTML_DIR.exists():
        shutil.rmtree(HTML_DIR)

    HTML_DIR.mkdir(parents=True)

    # copy everything except templates directory
    for item in ASSETS_DIR.iterdir():
        if item.name == "templates":
            continue

        if item.name == "pages":
            continue

        for file in item.iterdir():
            shutil.copy(file, HTML_DIR / file.name)

    print(f"Assets copied successfully")

def find_md_pages():
    md_pages = []

    root = ET.parse(XML_DIR / "index.xml").getroot()
    for cp in root.iterfind("compound"):
        if cp.get("kind") == "page":
            refid = cp.get("refid")
            md_pages.append(refid)

    return md_pages

def read_group_inner(group_def, tag):
    items = []
    for element in group_def.findall(tag):
        items.append((element.get("refid"), element.text))

    return items

def read_section_inner(group_def, kind):
    items = []
    for element in group_def.findall(".//memberdef"):
        if element.get("kind") == kind:
            items.append((element.get("id"), element.find("name").text))

    return items

def get_module(refid):
    module["functions"] = read_section_inner(group_def, "function")
    module["structs"] = read_group_inner(group_def, "innerclass")
    module["types"] = read_section_inner(group_def, "typedef")
    module["constants"] = read_section_inner(group_def, "define")
    module["subgroups"] = read_group_inner(group_def, "innergroup")

    return module

def get_md_page(refid):
    xml_file = ET.parse(XML_DIR / f"{refid}.xml").getroot()
    group_def = xml_file.find("compounddef")

    md_page = {}
    md_page["refid"] = refid
    md_page["name"] = group_def.findtext("compoundname")
    md_page["title"] = group_def.findtext("title")
    md_page["content"] = group_def.find("detaileddescription")

    return md_page

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

def build_guides_list(guides):
    for refid in guides:
        if refid in ["guides", "api_reference", "indexpage"]:
            continue

        md_page = get_md_page(refid)
        link = f"{md_page['refid']}.html"
        GUIDES.append(f'<li><a href="{link}">{md_page['title']}</a></li>')

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

    for refid in md_pages:
        md_page = get_md_page(refid)
        output_name = HTML_DIR / f"{md_page['refid']}.html"
        if refid == "indexpage":
            output_name = HTML_DIR / "index.html"
            
        write_docs_page(output_name, build_md_page(md_page))

    # generate html pages from source
    pages_dir = ASSETS_DIR / "pages"
    for file in pages_dir.iterdir():
        write_website_page(file.name, read_html_page(file))

    print(f"")
    print(f"Documentation generated successfully")

    return 0

if __name__ == "__main__":
    main()