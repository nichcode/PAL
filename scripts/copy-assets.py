#!/usr/bin/env python3

from pathlib import Path
import shutil


def main():
    root_dir = Path(__file__).resolve().parents[1]
    assets_dir = root_dir / "assets"
    output_dir = root_dir / "build/website/html"

    output_dir.mkdir(parents=True, exist_ok=True)
    
    for item in assets_dir.iterdir():
        if item.name == "templates":
            continue
        elif item.name == "pages":
            continue

        for file in item.iterdir():
            shutil.copy(file, output_dir / file.name)

    print(f"assets copied successfully")


if __name__ == "__main__":
    main()
