
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

        # for bit, name in self.values.items():
        #     print(f"bit={bit} name={name}")
        #     if bit != 0 and value & bit:
        #         print(f"bit={bit} name={name}")
        #         names.append(name)

        # return " | ".join(names)

def lookup(value):
    type_name = str(value.type)

    if type_name in sets:
        return SetPrinter(value, sets[type_name])
    
    if type_name in bitmask:
        return BitmaskPrinter(value, bitmask[type_name])
    
    return None

add_set("Status", {
    0: "STATUS_GOOD",
    1: "STATUS_BAD",
    2: "STATUS_NORMAL",
})

add_bitmask("Result", {
    0: "RESULT_SUCCESS",
    1 << 0: "RESULT_FAILED_IO",
    1 << 1: "RESULT_FAILED_HANDLE",
})

gdb.pretty_printers.append(lookup)