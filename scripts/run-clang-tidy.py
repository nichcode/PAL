#!/usr/bin/env python3

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