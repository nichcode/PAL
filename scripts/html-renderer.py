#!/usr/bin/env python3

from pathlib import Path
import pickle
import sys
import shutil

import re
from jinja2 import Environment, FileSystemLoader
from models import *


class HTMLRenderer:
    def __init__(self, root_dir, modules) -> None:
        self.root_dir = root_dir
        self.modules: list[Module] = modules

        self.env = Environment(loader=FileSystemLoader(root_dir / "assets/templates"))
        self.website_page_template = self.env.get_template("website_page.html")
        self.docs_page_template = self.env.get_template("docs_page.html")

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
        """Copies pages with sources from assets/pages directory"""
        pages_dir = self.root_dir / "assets/pages"
        output_dir = self.root_dir / "build/website"

        for file in pages_dir.iterdir():
            text = Path(file).read_text(encoding="utf-8")
            title, body = text.split("\n", 1)
            output_file = output_dir / file.name

            page = self.website_page_template.render(title=title, content=body)
            output_file.write_text(page, encoding="utf-8")

        print("website pages generated successfully")


def main():
    root_dir = Path(__file__).resolve().parents[1]
    filepath = root_dir / "build/modules.pkl"

    try:
        with open(filepath, "rb") as file:
            modules: list[Module] = pickle.load(file)

    except (pickle.PickleError, EOFError, AttributeError) as e:
        print(f"modules cache is corrupted or out of date: {e}.")
        sys.exit(1)

    renderer = HTMLRenderer(root_dir, modules)
    renderer.copy_assets()
    renderer.generate_website_pages()

if __name__ == "__main__":
    main()