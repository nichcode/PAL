#!/usr/bin/env python3

import sys
import os
import subprocess
from pathlib import Path

import shutil
import re
from jinja2 import Environment, FileSystemLoader
from collections import defaultdict

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

def build_guides_list():
    GUIDES.append('<li><a href="getting-started.html">Getting started</a></li>')
    GUIDES.append('<li><a href="testing1.html">Testing 1</a></li>')
    GUIDES.append('<li><a href="testing2.html">Testing 2</a></li>')

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
    tmp = docs_page_template.render(
        title=page["title"],
        content=page["content"],
    )

    guides_content = "\n".join(GUIDES)
    html_page = tmp.replace("<!-- guides -->", guides_content)
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

    # get xml.index and group by kind
    root = ET.parse(XML_DIR / "index.xml").getroot()
    kinds = defaultdict(list)
    for cp in root.iterfind("compound"):
        kinds[cp.get("kind")].append((cp.get("refid"), cp.findtext("name")))

    # open the group files and cache child groups
    for refid, _ in kinds["group"]:
        xml_file = ET.parse(XML_DIR / f"{refid}.xml").getroot()
        group_def = xml_file.find("compounddef")

        subgroups = []
        for inner in group_def.findall("innergroup"):
            subgroup_refid = inner.get("refid")
            subgroups.append(subgroup_refid)
            children.append(subgroup_refid)

        # check if a group is a module
        modules = []
        for refid, _ in kinds["group"]:
            if refid not in children:
                modules.append(refid)

    return modules

def read_inner(group_def, tag):
    items = []
    for element in group_def.findall(tag):
        refid = element.get("refid")
        name = element.text
        items.append((refid, name))

    return items

def get_module(refid):
    xml_file = ET.parse(XML_DIR / f"{refid}.xml").getroot()
    group_def = xml_file.find("compounddef")

    module = {}
    module["refid"] = refid
    module["name"] = group_def.findtext("compoundname")
    module["title"] = group_def.findtext("title")
    module["structs"] = read_inner(group_def, "innerclass")
    module["subgroups"] = read_inner(group_def, "innergroup")

    return module

def read_module(module):
    content = []
    content.append(f"<h1>{html.escape(module['title'])}</h1>")

    if module["structs"]:
        content.append("<h2>Structs</h2><ul>")
        for _, name in module["structs"]:
            content.append(f"<li>{html.escape(name)}</li>")
        content.append("</ul>")

    if module["subgroups"]:
        content.append("<h2>Constant Groups</h2><ul>")
        for _, name in module["subgroups"]:
            content.append(f"<li>{html.escape(name)}</li>")
        content.append("</ul>")

    content_str = "".join(content)
    return {"title": module["title"], "content": content_str}

def main():
    os.chdir(DOCS_DIR) # change working directory so doxygen works as intended
    HTML_DIR.mkdir(parents=True, exist_ok=True)
    generate_doxygen()

    build_guides_list()
    modules = find_modules()
    copy_assets()

    for refid in modules:
        module = get_module(refid)
        write_docs_page(HTML_DIR / f"{module['refid']}.html", read_module(module))

    # generate html pages from source
    pages_dir = ASSETS_DIR / "pages"
    for file in pages_dir.iterdir():
        write_website_page(file.name, read_html_page(file))

    print(f"")
    print(f"Documentation generated successfully")

    return 0

if __name__ == "__main__":
    main()