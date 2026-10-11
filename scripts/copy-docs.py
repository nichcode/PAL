#!/usr/bin/env python3

import shutil
import os
import sys
from pathlib import Path

def main():
    root_dir = Path(__file__).resolve().parents[1]
    docs_path = root_dir / "docs" / "html"
    base_path = docs_path / "index.html"
    if not os.path.isfile(base_path):
        print(f"Failed to find html/index.html: {base_path}")
        sys.exit(1)

    dest_path = root_dir / "website/html"
    shutil.copytree(docs_path, dest_path, dirs_exist_ok=True)
    print(f"Documentation copied successfully")

if __name__ == "__main__":
    main()