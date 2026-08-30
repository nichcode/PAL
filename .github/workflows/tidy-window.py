
import os
import json
import subprocess
import sys

def main():
    path = "compile_commands.json"
    if not os.path.exists(path):
        print(f"Failed to {path}")
        sys.exit(1)

if __name__ == "__main__":
    main()