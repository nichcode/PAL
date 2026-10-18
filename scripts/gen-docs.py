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

def parse_md_page_contents(md_page):
    page = []
    page.append(f"<h1>{html.escape(md_page['title'])}</h1>")

    for element in md_page['content']:
        if element.tag == "para":
            # check if the para line contains a table
            table = element.find("table")
            if table is not None:
                page.append("<table>")

                for row in table.findall("row"):
                    page.append("  <tr>")
                    for entry in row.findall("entry"):
                        # check whether this item is a table head
                        tag = "td"
                        if entry.get("thead") == "yes":
                            tag = "th"
                        
                        cell_para = entry.find("para")
                        cell_text = ""

                        if cell_para is not None:
                            if cell_para.text:
                                cell_text += cell_para.text
                            
                            for child in cell_para:
                                # cross-reference page links
                                if child.tag == "ref":
                                    ref_id = child.get("refid")
                                    cell_text += f'<a href="{ref_id}.html">{child.text}</a>'

                                if child.tail:
                                    cell_text += child.tail
                                    
                        page.append(f"    <{tag}>{cell_text.strip()}</{tag}>")

                    page.append("  </tr>")
                page.append("</table>")

            else:
                # Handle plain paragraph strings normally
                para_text = ""
                if element.text:
                    para_text = element.text

                for child in element:
                    if child.tag == "anchor":
                        para_text += f'<span id="{child.get("id")}"></span>'

                    elif child.tag == "computeroutput":
                        para_text += f"<code>{child.text}</code>"

                    elif child.tag == "ulink":
                        para_text += f'<a href="{child.get("url")}">{child.text}</a>'

                    if child.tail:
                        para_text += child.tail

                page.append(f"<p>{para_text.strip()}</p>")

    return "\n".join(page)

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

def generate_doxygen():
    subprocess.run(["doxygen", "doxyfile"])

    log_file = BUILD_DIR / "doxygen.log"
    with open(log_file, "r") as file:
        if not file:
            print(f"")
            print(f"Failed to load doxygen.log: {log_file}")
            sys.exit(1)

        # remove all macro warnings
        file_contents = file.readlines()
        warnings = []
        tags = []
        tags.append("(macro definition)")
        tags.append("(variable) of struct PalGraphicsBackendVtable1")
        tags.append("(variable) of struct PalGraphicsBackendVtable2")

        for line in file_contents:
            found = False

            for tag in tags:
                if tag in line:
                    found = True
                    break

            if not found:
                warnings.append(line)
    
    # write the clean warnings to the file
    with open(log_file, "w") as file:
        file.writelines(warnings);
    
    if warnings:
        print(f"")
        print(f"There are errors with the documentation generation")
        print(f"See {log_file} for the errors and fix them")
        sys.exit(1)

def find_modules():
    children = []

    root = ET.parse(XML_DIR / "index.xml").getroot()
    groups = []
    for cp in root.iterfind("compound"):
        if cp.get("kind") == "group":
            groups.append((cp.get("refid"), cp.findtext("name")))

    # open the group files and cache child groups
    for refid, _ in groups:
        xml_file = ET.parse(XML_DIR / f"{refid}.xml").getroot()
        group_def = xml_file.find("compounddef")

        # find nested sub-groups
        for inner in group_def.findall("innergroup"):
            subgroup_refid = inner.get("refid")
            children.append(subgroup_refid)

    # filter modules
    modules = []
    for refid, _ in groups:
        if refid not in children:
            modules.append(refid)

    return modules

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
    xml_file = ET.parse(XML_DIR / f"{refid}.xml").getroot()
    group_def = xml_file.find("compounddef")

    module = {}
    module["refid"] = refid
    module["name"] = group_def.findtext("compoundname")
    module["title"] = group_def.findtext("title")

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

    if module["functions"]:
        content.append("<h2>Functions</h2><ul>")
        for _, name in module["functions"]:
            content.append(f"<li>{html.escape(name)}</li>")
        content.append("</ul>")

    if module["structs"]:
        content.append("<h2>Structs</h2><ul>")
        for _, name in module["structs"]:
            content.append(f"<li>{html.escape(name)}</li>")
        content.append("</ul>")

    if module["types"]:
        content.append("<h2>Types</h2><ul>")
        for _, name in module["types"]:
            content.append(f"<li>{html.escape(name)}</li>")
        content.append("</ul>")

    if module["subgroups"] or module["constants"]:
        content.append("<h2>Constants</h2><ul>")

        for _, name in module["constants"]:
            content.append(f"<li>{html.escape(name)}</li>")

        for _, name in module["subgroups"]:
                content.append(f"<li>{html.escape(name)}</li>")

        content.append("</ul>")

    return {"title": module["title"], "content": "\n".join(content)}

def build_md_page(md_page):
    return {"title": md_page["title"], "content": parse_md_page_contents(md_page)}

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
        link = f"{module['refid']}.html"
        title = module['title']
        tmp_link = "core-types.html"

        MODULES.append('<li class="sidebar-group">')

        MODULES.append('<div class="sidebar-heading">')
        MODULES.append('<button class="sidebar-arrow" aria-expanded="false"></button>')
        MODULES.append(f'<a href="{link}">{title}</a>')
        MODULES.append('</div>')
        MODULES.append('<ul class="sidebar-tree" hidden>')

        MODULES.append('<li class="sidebar-group">')
        MODULES.append('<div class="sidebar-heading">')
        MODULES.append('<button class="sidebar-arrow" aria-expanded="false"></button>')
        MODULES.append(f'<a href="{tmp_link}">Functions</a>')
        MODULES.append('</div>')

        MODULES.append('<ul class="sidebar-tree" hidden>')
        if module["functions"]:
            for _, name in module["functions"]:
                MODULES.append(f'<li><a href="{tmp_link}">{name}</a></li>')
        MODULES.append('</ul>')
        MODULES.append('</li>')

        MODULES.append('<li class="sidebar-group">')
        MODULES.append('<div class="sidebar-heading">')
        MODULES.append('<button class="sidebar-arrow" aria-expanded="false"></button>')
        MODULES.append(f'<a href="{tmp_link}">Structs</a>')
        MODULES.append('</div>')

        MODULES.append('<ul class="sidebar-tree" hidden>')
        if module["structs"]:
            for _, name in module["structs"]:
                MODULES.append(f'<li><a href="{tmp_link}">{name}</a></li>')
        MODULES.append('</ul>')
        MODULES.append('</li>')

        MODULES.append('<li class="sidebar-group">')
        MODULES.append('<div class="sidebar-heading">')
        MODULES.append('<button class="sidebar-arrow" aria-expanded="false"></button>')
        MODULES.append(f'<a href="{tmp_link}">Types</a>')
        MODULES.append('</div>')

        MODULES.append('<ul class="sidebar-tree" hidden>')
        if module["types"]:
            for _, name in module["types"]:
                MODULES.append(f'<li><a href="{tmp_link}">{name}</a></li>')
        MODULES.append('</ul>')
        MODULES.append('</li>')

        MODULES.append('<li class="sidebar-group">')
        MODULES.append('<div class="sidebar-heading">')
        MODULES.append('<button class="sidebar-arrow" aria-expanded="false"></button>')
        MODULES.append(f'<a href="{tmp_link}">Constants</a>')
        MODULES.append('</div>')

        MODULES.append('<ul class="sidebar-tree" hidden>')

        if module["constants"]:
            for _, name in module["constants"]:
                MODULES.append(f'<li><a href="{tmp_link}">{name}</a></li>')

        if module["subgroups"]:
                    for _, name in module["subgroups"]:
                        MODULES.append(f'<li><a href="{tmp_link}">{name}</a></li>')

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