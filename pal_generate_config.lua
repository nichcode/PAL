
dofile("pal_config.lua")

local function writeConfig(path)
    local file = io.open(path, "w")
    file:write("\n// Auto Generated Config Header\n\n")

    if (PAL_BUILD_VIDEO) then
        file:write("#define PAL_HAS_VIDEO 1\n")
    end

    if (PAL_BUILD_EVENT) then
        file:write("#define PAL_HAS_EVENT 1\n")
    end

    file:close()
end

local function writeCppProperties(path)
    local defines = {}

    table.insert(defines, "_UNICODE")
    table.insert(defines, "UNICODE")

    if (PAL_BUILD_VIDEO) then
        table.insert(defines, "PAL_HAS_VIDEO = 1")
    end

    if (PAL_BUILD_EVENT) then
        table.insert(defines, "PAL_HAS_EVENT = 1")
    end

    if not (PAL_BUILD_STATIC) then
        table.insert(defines, "_PAL_EXPORT")
    end

    --properties
    local properties = {
        configurations = {
            {
                name = "Windows",
                includePath = {
                    "${workspaceFolder}/include",
                    "${workspaceFolder}/src"
                },
                defines = defines,
                compilerPath = "C:/msys64/ucrt64/bin/gcc.exe",
                cStandard = "c99",
                cppStandard = "c++17",
                intelliSenseMode = "gcc-x64"
            }
        },
        version = 4
    }

    local json = require("json-lua/json")
    local out = json.encode(properties, {index = 4})
    
    local f = io.open(path, "w")
    f:write(out)
    f:close()
end

return {
    writeConfigHeader = writeConfig,
    writeProperties = writeCppProperties
}