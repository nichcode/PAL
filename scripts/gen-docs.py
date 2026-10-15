#!/usr/bin/env python3

import sys
import os
import subprocess
from pathlib import Path
import shutil
import re
from jinja2 import Environment, FileSystemLoader

ROOT_DIR = root_dir = Path(__file__).resolve().parents[1]
ASSETS_DIR = root_dir / "assets"
OUTPUT_DIR = root_dir / "website/html"

env = Environment(loader=FileSystemLoader(ASSETS_DIR))
page_template = env.get_template("page.html")

def read_html_page(filepath):
    text = Path(filepath).read_text(encoding="utf-8")

    # check for the tile of the page
    # this must be on the first line and in the format <!-- title: site tile -->
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
    html = page_template.render(
        title=page["title"],
        content=page["content"],
        sidebar=sidebar,
        root=""
    )
    file = OUTPUT_DIR / filename
    file.write_text(html, encoding="utf-8")

def copy_assets():
    if OUTPUT_DIR.exists():
        shutil.rmtree(OUTPUT_DIR)

    OUTPUT_DIR.mkdir(parents=True)

    # copy everything except page.html template file
    # this does not copy subfolders
    for item in ASSETS_DIR.iterdir():
        if item.name == "page.html":
            continue

        shutil.copy(item, OUTPUT_DIR / item.name)

    print(f"Assets copied successfully")

def main():
    page = read_html_page(ROOT_DIR / "website/tmp.html")
    write_html_page("tmp.html", page)

    # copy_assets()

    # root_dir = Path(__file__).resolve().parents[1]
    # build_path = root_dir / "build"
    # build_path.mkdir(parents=True, exist_ok=True)

    # os.chdir(root_dir / "docs")
    # subprocess.run(["doxygen", "doxyfile"])

    # log_file = build_path / "doxygen.log"
    # with open(log_file, "r") as file:
    #     if not file:
    #         print(f"")
    #         print(f"Failed to load doxygen.log: {log_file}")
    #         sys.exit(1)

    #     # remove all macro warnings
    #     file_contents = file.readlines()
    #     warnings = []
    #     tags = []
    #     tags.append("(macro definition)")
    #     tags.append("(variable) of struct PalGraphicsBackendVtable1")
    #     tags.append("(variable) of struct PalGraphicsBackendVtable2")

    #     for line in file_contents:
    #         found = False

    #         for tag in tags:
    #             if tag in line:
    #                 found = True
    #                 break

    #         if not found:
    #             warnings.append(line)
    
    # # write the clean warnings to the file
    # with open(log_file, "w") as file:
    #     file.writelines(warnings);
    
    # if warnings:
    #     print(f"")
    #     print(f"There are errors with the documentation generation")
    #     print(f"See {log_file} for the errors and fix them")
    #     sys.exit(1)

    # print(f"")
    # print(f"Documentation generated successfully")
    return 0

if __name__ == "__main__":
    main()