
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
from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path

def worker(info):
    program, name = info
    result = subprocess.run([str(program)])
    return name, result.returncode

def main():
    root_dir = Path(__file__).resolve().parents[1]
    path = root_dir / "build/tests.json"

    if not os.path.exists(path):
        print(f"Failed to find tests.json: {path}")
        sys.exit(1)
    
    buildpath = root_dir / "bin/debug"
    if len(sys.argv) > 1:
        if sys.argv[1] == "release":
            buildpath = root_dir / "bin/release"

    with open(path, "r") as file:
        files = json.load(file)

    print(f"Found {len(files)} tests. Running parallel...")
    print(f"")

    extention = ""
    if sys.platform == "win32":
        extention = ".exe"

    with ThreadPoolExecutor() as pool:
        workers = []
        for f in files:
            program = buildpath / (f + extention)
            info = (program, f)
            workers.append(pool.submit(worker, info))

    passed = 0
    failed = 0

    for w in as_completed(workers):
        name, code = w.result()
        if code == 0:
            print(f"{name}: PASSED")
            passed += 1
        else:
            print(f"{name}: FAILED")
            failed += 1

    print(f"")
    print(f"Test: {len(files)}")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")

if __name__ == "__main__":
    main()