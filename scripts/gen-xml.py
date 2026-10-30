#!/usr/bin/env python3

import sys
import os
import subprocess
from pathlib import Path


def main():
    root_dir = Path(__file__).resolve().parents[1]
    log_file = root_dir / "build/doxygen.log"
    os.chdir(root_dir / "docs")

    subprocess.run(["doxygen", "doxyfile"])

    with open(log_file, "r") as file:
        if not file:
            print(f"")
            print(f"Failed to load doxygen.log: {log_file}")
            sys.exit(1)

        # remove all lines with any of the tags
        file_contents = file.readlines()
        warnings = []
        tags = ["(macro definition)"]

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
        file.writelines(warnings)

    if warnings:
        print(f"There are errors with the documentation generation")
        print(f"See {log_file} for the errors and fix them")
        sys.exit(1)

    print("xml files generated successfully")


if __name__ == "__main__":
    main()
