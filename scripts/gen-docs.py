#!/usr/bin/env python3

import sys
import os
import subprocess
from pathlib import Path

def main():
    root_dir = Path(__file__).resolve().parents[1]
    build_path = root_dir / "build"
    build_path.mkdir(parents=True, exist_ok=True)

    os.chdir(root_dir / "docs")
    subprocess.run(["doxygen", "doxyfile"])

    log_file = build_path / "doxygen.log"
    with open(log_file, "r") as file:
        if not file:
            print(f"")
            print(f"Failed to load doxygen.log: {log_file}")
            sys.exit(1)

        if not file.read(1) == "":
            print(f"")
            print(f"There are errors with the documentation generation")
            print(f"See {log_file} for the errors and fix them")
            sys.exit(1)

    print(f"")
    print(f"Documentation generated successfully")
    return 0

if __name__ == "__main__":
    main()