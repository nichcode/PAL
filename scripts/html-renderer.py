#!/usr/bin/env python3

from pathlib import Path
import pickle
import sys
from models import *

def main():
    root_dir = Path(__file__).resolve().parents[1]
    filepath = root_dir / "build/modules.pkl"

    try:
        with open(filepath, "rb") as file:
            modules: list[Module] = pickle.load(file)

    except (pickle.PickleError, EOFError, AttributeError) as e:
        print(f"modules cache is corrupted or out of date: {e}.")
        sys.exit(1)

    for module in modules:
        print(module.name)

if __name__ == "__main__":
    main()