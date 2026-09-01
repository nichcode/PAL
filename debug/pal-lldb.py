
import lldb # type: ignore

sets = {}

def addSet(name, values):
    sets[name] = values

def summary(value, _):
    name = value.GetType().GetName()
    values = sets.get(name)

    if values is None:
        return None

    num = value.GetValueAsUnsigned()
    return values.get(num, str(num))

def __lldb_init_module(debugger, internal_dict):
    for name in sets:
        debugger.HandleCommand(f'type summary add -F {__name__}.summary {name}')
