
# PAL - Prime Abstraction Layer (PAL)
# A cross platform abstraction layer over graphics and windowing APIs
# -------------------------------------------------------------------
# 
# Copyright (C) 2025-2026 Nicholas Agbo <agbonicholas04@gmail.com>
#
# This software is provided 'as-is', without any express or implied
# warranty.  In no event will the authors be held liable for any damages
# arising from the use of this software.
#
# Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it
# freely, subject to the following restrictions:
#
# 1. The origin of this software must not be misrepresented; you must not
#    claim that you wrote the original software. If you use this software
#    in a product, an acknowledgment in the product documentation would be
#    appreciated but is not required.
#
# 2. Altered source versions must be plainly marked as such, and must not be
#    misrepresented as being the original software.
#
# 3. This notice may not be removed or altered from any source distribution.

import os
import json
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

def worker(info):
    file, buildpath = info
    cmd = ["clang-tidy", file, "-p", buildpath]
    subprocess.run(cmd, shell=False)

def main():
    root_dir = Path(__file__).resolve().parents[1]
    print(root_dir)
    path = root_dir / "build/compile_commands.json"

    if not os.path.exists(path):
        print(f"Failed to find compile_commands.json: {path}")
        sys.exit(1)

    buildpath = os.path.dirname(path)
    with open(path, "r") as file:
        commands = json.load(file)

    files = []
    for entry in commands:
        files.append(os.path.normpath(entry["file"]))

    print(f"Found {len(files)} targets. Running parallel...")

    workers = []
    for f in files:
        workers.append((f, buildpath))

    with ThreadPoolExecutor() as pool:
        pool.map(worker, workers)

if __name__ == "__main__":
    main()