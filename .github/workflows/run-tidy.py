
import os
import json
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor

def worker(info):
    file, path = info
    print(f"Checking: {file}")

    config = os.path.join(path, ".clang-tidy")
    cmd = ["clang-tidy", file, "-p", path, f"--config-file={config}"]
    subprocess.run(cmd, shell=False)

def main():
    if os.getenv("GITHUB_ACTIONS") == "true":
        path = os.path.abspath("compile_commands.json")
    else:
        path = os.path.abspath("../../compile_commands.json")

    if not os.path.exists(path):
        print(f"Failed to find compile_commands.json: {path}")
        sys.exit(1)

    buildpath = os.path.dirname(path)

    # Load the files
    with open(path, "r") as file:
        commands = json.load(file)

    files = []
    for entry in commands:
        files.append(os.path.normpath(entry["file"]))

    print(f"Found {len(files)} targets. Running parallel analysis...")

    workers = []
    for f in files:
        workers.append((f, buildpath))

    with ThreadPoolExecutor(max_workers=4) as pool:
        pool.map(worker, workers)


if __name__ == "__main__":
    main()