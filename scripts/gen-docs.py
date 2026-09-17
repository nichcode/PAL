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

        # remove all macro warnings
        file_contents = file.readlines()
        warnings = []
        tag = "(macro definition)"

        for line in file_contents:
            if tag not in line:
                warnings.append(line);
    
    # write the clean warnings to the file
    with open(log_file, "w") as file:
        file.writelines(warnings);
    
    if warnings:
        print(f"")
        print(f"There are errors with the documentation generation")
        print(f"See {log_file} for the errors and fix them")
        sys.exit(1)

    print(f"")
    print(f"Documentation generated successfully")
    return 0

if __name__ == "__main__":
    main()