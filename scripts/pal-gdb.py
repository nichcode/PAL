
import gdb # type: ignore

sets = {}

def addSet(name, values):
    sets[name] = values

class SetPrinter:
    def __init__(self, value, values):
        self.value = value
        self.values = values

    def to_string(self):
        value = int(self.value)
        return self.values.get(value, str(value))

def lookup(value):
    values = sets.get(str(value.type))
    if values is None:
        return None

    return SetPrinter(value, values)

addSet("Status", {
    0: "STATUS_GOOD",
    1: "STATUS_BAD",
    2: "STATUS_NORMAL",
})

gdb.pretty_printers.append(lookup)