#!/usr/bin/env python3

from pathlib import Path
import pickle
import sys
import shutil
from models import *


class HTMLRenderer:
    def __init__(self, root_dir, modules) -> None:
        self.root_dir = root_dir
        self.modules: list[Module] = modules

    def copy_assets(self):
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

    def generate_html_pages(self):
        print("a")


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
    renderer.generate_html_pages()

if __name__ == "__main__":
    main()