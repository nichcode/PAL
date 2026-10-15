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

ROOT_DIR = Path(__file__).resolve().parents[1]
ASSETS_DIR = ROOT_DIR / "assets"
WEBSITE_DIR = ROOT_DIR / "website"
HTML_DIR = ROOT_DIR / "website/html"
BUILD_DIR = ROOT_DIR / "build"
DOCS_DIR = ROOT_DIR / "docs"
XML_DIR = BUILD_DIR / "xml"

env = Environment(loader=FileSystemLoader(ASSETS_DIR))
page_template = env.get_template("page.html")

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

def write_html_page(filename, page, sidebar=""):
    base = HTML_DIR.relative_to(WEBSITE_DIR)
    root = base.as_posix() + "/"
    html = page_template.render(
        title=page["title"],
        content=page["content"],
        sidebar=sidebar,
        root=root
    )
    file = WEBSITE_DIR / filename
    file.write_text(html, encoding="utf-8")

def copy_assets():
    if HTML_DIR.exists():
        shutil.rmtree(HTML_DIR)

    HTML_DIR.mkdir(parents=True)

    # copy everything except page.html template file
    # this does not copy subfolders
    for item in ASSETS_DIR.iterdir():
        if item.suffix == ".html":
            continue

        shutil.copy(item, HTML_DIR / item.name)

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

def group_by_kind():
    xml_base = ET.parse(XML_DIR / "index.xml").getroot()
    kinds = defaultdict(list)
    for cp in xml_base.iterfind("compound"):
        kinds[cp.get("kind")].append((cp.get("refid"), cp.findtext("name")))
    
    return kinds

def get_group_tree(kinds):
    children = {}
    groups_children = []

    # open the group files and cache child groups
    for refid, _ in kinds["group"]:
        xml_file = ET.parse(XML_DIR / f"{refid}.xml").getroot()
        group_def = xml_file.find("compounddef")

        subgroups = []
        for inner in group_def.findall("innergroup"):
            subgroup_refid = inner.get("refid")
            subgroups.append(subgroup_refid)
            groups_children.append(subgroup_refid)

        children[refid] = subgroups

        # check if a group is a module
        modules = []
        for refid, _ in kinds["group"]:
            if refid not in groups_children:
                modules.append(refid)

    return modules, children

def read_inner(group_def, tag):
    items = []
    for element in group_def.findall(tag):
        refid = element.get("refid")
        name = element.text
        items.append((refid, name))

    return items

def read_module(refid):
    xml_file = ET.parse(XML_DIR / f"{refid}.xml").getroot()
    group_def = xml_file.find("compounddef")

    module = {}
    module["refid"] = refid
    module["name"] = group_def.findtext("compoundname")
    module["title"] = group_def.findtext("title")
    module["structs"] = read_inner(group_def, "innerclass")
    module["subgroups"] = read_inner(group_def, "innergroup")

    return module

def main():
    os.chdir(DOCS_DIR) # change working directory so doxygen works as intended
    BUILD_DIR.mkdir(parents=True, exist_ok=True)
    WEBSITE_DIR.mkdir(parents=True, exist_ok=True)

    # generate_doxygen()

    kinds = group_by_kind()
    modules, children = get_group_tree(kinds)

    for refid in modules:
        module = read_module(refid)
        print("module: ", module["title"], "(" + module["name"] + ")")
        print(" structs: ", module["structs"])
        print(" subgroups: ", module["subgroups"])

    # generate html pages from source
    # for file in ASSETS_DIR.glob("*.html"):
    #     if file.name == "page.html":
    #         continue

    #     write_html_page(file.name, read_html_page(file))
    
    # copy_assets()

    print(f"")
    print(f"Documentation generated successfully")

    return 0

if __name__ == "__main__":
    main()