
import gdb # type: ignore

sets = {}
bitmask = {}

def add_set(name, values):
    sets[name] = values

def add_bitmask(name, values):
    bitmask[name] = values

class SetPrinter:
    def __init__(self, value, values):
        self.value = value
        self.values = values

    def to_string(self):
        value = int(self.value)
        return self.values.get(value, str(value))
    
class BitmaskPrinter:
    def __init__(self, value, values):
        self.value = value
        self.values = values

    def to_string(self):
        value = int(self.value)
        if value == 0:
            return self.values.get(0, "0")
        
        names = []
        unknowns = []

        for bit in range(value.bit_length()):
            mask = 1 << bit
            if not (value & mask):
                continue

            name = self.values.get(mask)
            if name is not None:
                names.append(name)
            else:
                unknowns.append(mask)
        
        for bit in unknowns:
            names.append(f"0x{bit:x}")

        return " | ".join(names)

def lookup(value):
    type_name = str(value.type)

    if type_name in sets:
        return SetPrinter(value, sets[type_name])
    
    if type_name in bitmask:
        return BitmaskPrinter(value, bitmask[type_name])
    
    return None

# Core
add_set("PalBool", {
    0: "false",
    1: "true"
})

add_set("PalResult", {
    0: "PAL_RESULT_SUCCESS"
})

add_set("PalResultCode", {
    0: "PAL_RESULT_CODE_NONE",
    1: "PAL_RESULT_CODE_INVALID_ARGUMENT",
    2: "PAL_RESULT_CODE_OUT_OF_MEMORY",
    3: "PAL_RESULT_CODE_PLATFORM_FAILURE",
    4: "PAL_RESULT_CODE_TIMEOUT",
    5: "PAL_RESULT_CODE_INVALID_HANDLE",
    6: "PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED",
    7: "PAL_RESULT_CODE_INVALID_OPERATION",
    8: "PAL_RESULT_CODE_DEVICE_LOST",
    9: "PAL_RESULT_CODE_OUT_OF_DATE",
    10: "PAL_RESULT_CODE_COUNT"
})

add_set("PalResultSource", {
    0: "PAL_RESULT_SOURCE_NONE",
    1: "PAL_RESULT_SOURCE_WIN32",
    2: "PAL_RESULT_SOURCE_POSIX",
    3: "PAL_RESULT_SOURCE_EGL",
    4: "PAL_RESULT_SOURCE_VULKAN",
    5: "PAL_RESULT_SOURCE_D3D12",
    6: "PAL_RESULT_SOURCE_METAL",
    7: "PAL_RESULT_SOURCE_COUNT"
})

gdb.pretty_printers.append(lookup)