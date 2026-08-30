
dofile("pal_config.lua")

targetDir = "%{wks.location}/bin/%{cfg.buildcfg}"
objDir = "%{wks.location}/build"

workspaceName = "PALWorkspace"
compilerPath = ""
intellisenseMode = ""
debuggerPath = ""
gccBasePath = ""

local function getCommandOutput(cmd)
    local result, exitCode = os.outputof(cmd)
    if result then
        result = result:gsub("[\r\n%s]+$", "")

        if (result ~= "") then
            result = result:gsub("\\", "/")
            return result
        else
            return nil
        end
    end
    return nil
end

local function escape(value)
    return value:gsub('\\', '\\\\'):gsub('"', '\\"')
end

local function removeDuplicates(list)
    local seen = {}
    local out = {}

    for _, v in ipairs(list) do
        if not seen[v] then
            seen[v] = true
            table.insert(out, v)
        end
    end

    return out
end

local function generateVscodeProperties()
    print("\n=======================================================")
    print("Generating .vscode/c_cpp_properties.json")

    local prjDefines = {}
    local prjIncludes = {}
    local workspace = premake.global.getWorkspace(workspaceName)

    for prj in premake.workspace.eachproject(workspace) do
        for _, path in ipairs(prj.includedirs or {}) do
            table.insert(prjIncludes, path)
        end

        for _, define in ipairs(prj.defines or {}) do
            table.insert(prjDefines, define)
        end
    end

    -- remove duplicates
    prjIncludes = removeDuplicates(prjIncludes)
    prjDefines = removeDuplicates(prjDefines)

    -- write to file
    os.mkdir(".vscode") -- ensure .vscode directory exists
    local file = io.open(".vscode/c_cpp_properties.json", "w")
    if file then
        file:write('{\n')
        file:write('    "configurations": [\n')
        file:write('        {\n')
        file:write(string.format('            "name": "%s",\n', workspaceName))

        -- includes
        file:write('            "includePath": [\n')
        for i, dir in ipairs(prjIncludes) do
            file:write(string.format('                "%s"%s\n', dir, i < #prjIncludes and "," or ""))
        end
        file:write('            ],\n')

        -- defines
        file:write('            "defines": [\n')
        for i, define in ipairs(prjDefines) do
            file:write(string.format('                "%s"%s\n', define, i < #prjDefines and "," or ""))
        end
        file:write('            ],\n')

        file:write(string.format('            "compilerPath": "%s",\n', compilerPath))
        file:write(string.format('            "intelliSenseMode": "%s",\n', intellisenseMode))
        file:write('            "cStandard": "c99"\n')

        file:write('        }\n')
        file:write('    ],\n')
        file:write('    "version": 4\n')
        file:write('}\n')

        file:close()
    end
end

local function writeTasksConfiguration(file, actionType)
    local name = ""
    local isDefault = "false"
    local command = ""

    if actionType == "buildDebug" then
        name = "build debug"
        command = "make all -j config=debug"
        isDefault = "true"

    elseif actionType == "buildRelease" then
        name = "build release"
        command = "make all -j config=release"

    elseif actionType == "cleanDebug" then
        name = "clean debug"
        command = "make -j clean config=debug"

    elseif actionType == "cleanRelease" then
        name = "clean release"
        command = "make -j clean config=release"
    end
    
    file:write("        {\n")
    file:write('            "type": "shell",\n')
    file:write(string.format('            "label": "%s %s",\n', workspaceName, name))
    file:write(string.format('            "command": "%s",\n', command))

    file:write('            "options": {\n')
    file:write('                "cwd": "${workspaceFolder}"\n')
    file:write('            },\n')

    file:write('            "problemMatcher": [\n')
    file:write('                "$gcc",\n')
    file:write('            ],\n')

    file:write('            "group": {\n')
    file:write('                "kind": "build",\n')
    file:write(string.format('                "isDefault": %s\n', isDefault))
    file:write('            }\n')
end

local function generateTasksJson()
    print("\n=======================================================")
    print("Generating .vscode/tasks.json")

    local file = io.open(".vscode/tasks.json", "w")
    if file then
        file:write('{\n')
        file:write('    "tasks": [\n')
        
        writeTasksConfiguration(file, "buildDebug")
        file:write("        },\n")
        file:write('\n')

        writeTasksConfiguration(file, "buildRelease")
        file:write("        },\n")
        file:write('\n')

        -- clean configurations
        writeTasksConfiguration(file, "cleanDebug")
        file:write("        },\n")
        file:write('\n')

        writeTasksConfiguration(file, "cleanRelease")
        file:write("        }\n")

        file:write("    ],\n")
        file:write('    "version": "2.0.0"\n')
        file:write("}\n")

        file:close()
    end
end

local function writeLaunchConfiguration(file, app, isDebug)
    local name = ""
    local preLaunchTask = ""
    local dir = ""
    local cwd = ""

    if isDebug then
        name = "launch debug"
        preLaunchTask = "build debug"
        dir = "Debug"
    else
        name = "launch release"
        preLaunchTask = "build release"
        dir = "Release"
    end

    if app == "tests" then
        cwd = "tests"
        if isDebug then
            name = "launch tests debug"
        else
            name = "launch tests release"
        end

        if os.target() == "windows" then
            program = "tests.exe"
        else
            program = "tests"
        end

    else
        cwd = "tools/abi_dump"
        if isDebug then
            name = "launch abi-dump debug"
        else
            name = "launch abi-dump release"
        end

        if os.target() == "windows" then
            program = "abi-dump.exe"
        else
            program = "abi-dump"
        end
    end

    file:write("        {\n")
    file:write(string.format('            "name": "%s %s",\n', workspaceName, name))
    file:write('            "type": "cppdbg",\n')
    file:write('            "request": "launch",\n')
    file:write('            "stopAtEntry": false,\n')
    file:write(string.format('            "cwd": "${workspaceFolder}/%s",\n', cwd))

    file:write('            "environment": [],\n')
    file:write('            "externalConsole": false,\n')
    file:write(string.format('            "preLaunchTask": "%s %s",\n', workspaceName, preLaunchTask))
    file:write(string.format('            "program": "${workspaceFolder}/bin/%s/%s",\n', dir, program))
    file:write('            "MIMode": "gdb",\n')
    file:write(string.format('            "miDebuggerPath": "%s",\n', debuggerPath))

    if isDebug then
        file:write('            "setupCommands": [\n')

        file:write('                {\n')
        file:write('                    "description": "Enable pretty printing for gdb",\n')
        file:write('                    "text": "-enable-pretty-printing",\n')
        file:write('                    "ignoreFailures": false,\n')
        file:write('                },\n')

        file:write('                {\n')
        file:write('                    "description": "Set disassembly flavor to intel",\n')
        file:write('                    "text": "-gdb-set disassembly-flavor intel",\n')
        file:write('                    "ignoreFailures": false,\n')
        file:write('                }\n')

        file:write('            ]\n')
    end
end

local function generateLaunchJson()
    print("\n=======================================================")
    print("Generating .vscode/launch.json")

    local file = io.open(".vscode/launch.json", "w")
    if file then
        file:write('{\n')
        file:write('    "configurations": [\n')

        writeLaunchConfiguration(file, "tests", true)
        file:write("        },\n")
        file:write('\n')

        writeLaunchConfiguration(file, "tests", false)
        file:write("        },\n")
        file:write('\n')

        writeLaunchConfiguration(file, "abi-dump", true)
        file:write("        },\n")
        file:write('\n')

        writeLaunchConfiguration(file, "abi-dump", false)
        file:write("        }\n")

        file:write("    ],\n")
        file:write('    "version": "0.2.0"\n')
        file:write("}\n")
        file:close()
    end
end

local function generateSettingsJson()
    print("\n=======================================================")
    print("Generating .vscode/settings.json")

    local file = io.open(".vscode/settings.json", "w")
    if file then
        file:write('{\n')
        file:write('    "files.associations":\n')
        file:write('    {\n')
        file:write('        ".clang-tidy":"yaml",\n')
        file:write('        ".clang-format":"yaml"\n')

        file:write('    }\n')
        file:write('}\n')
        file:close()
    end
end

local function generateCompileCommands()
    print("\n=======================================================")
    print("Generating compile_commands.json")

    local file = io.open("compile_commands.json", "w")
    if not file then
        print("Failed to write to compile_commands.json")
    end

    file:write('[\n')

    local projects = {}
    local workspace = premake.global.getWorkspace(workspaceName)
    for prj in premake.workspace.eachproject(workspace) do
        table.insert(projects, prj)
    end

    for prjI, prj in ipairs(projects) do
        local flags = {}
        local isLastProject = prjI == #projects

        if (_ACTION == "gmake") then
            if (_OPTIONS["compiler"] == "clang") then
                if os.target() == "windows" then
                    table.insert(flags, "clang.exe")
                else
                    table.insert(flags, "clang")
                end
            else
                if os.target() == "windows" then
                    table.insert(flags, "gcc.exe")
                else
                    table.insert(flags, "gcc")
                end
            end

            table.insert(flags, " -g -m64 -std=c99 -Wno-switch -Wno-switch-enum")
            table.insert(flags, " -Wall -Wextra -Wpedantic -Wconversion")
            table.insert(flags, " -Wsign-conversion -Werror")

            if prj.kind == "SharedLib" then
                table.insert(flags, " -fPIC")
            else
                table.insert(flags, " -O3")
            end

            for _, path in ipairs(prj.includedirs or {}) do
                table.insert(flags, " -I " .. string.format('"%s"', path))
            end

            for _, define in ipairs(prj.defines or {}) do
                table.insert(flags, " -D " .. define)
            end
        end

        if (_ACTION == "vs2022") or (_ACTION == "vs2026") then
            table.insert(flags, "clang-cl.exe")
            table.insert(flags, " /Zi /std:c11 /W4 /WX /wd6387")
            table.insert(flags, " /wd4018 /wd4133 /wd4101")

            for _, path in ipairs(prj.includedirs or {}) do
                table.insert(flags, " /I " .. string.format('"%s"', path))
            end

            for _, define in ipairs(prj.defines or {}) do
                table.insert(flags, " /D " .. define)
            end
        end

        local cmdBase = table.concat(flags, "")
        for i, f in ipairs(prj.files) do
            local command = string.format('%s %s', cmdBase, f)
            file:write('    {\n')
            file:write(string.format('        "directory": "%s",\n', workspace.location))
            file:write(string.format('        "file": "%s",\n', f))
            file:write(string.format('        "command": "%s"\n', escape(command)))

            if i == #prj.files and isLastProject == true then
                file:write('    }\n')
            else
                file:write('    },\n')
                file:write('\n')
            end
        end
    end

    file:write(']\n')
    file:close()
end

-- generate vscode properties if using gmake
premake.override(premake.action, "call", function(base, action)
    base(action)

    if action == "gmake" then
        generateVscodeProperties()
        generateTasksJson()
        generateLaunchJson()
        generateSettingsJson()
    end
    generateCompileCommands()
    
end)

newoption {
    trigger = "compiler",
    description = "Choose a C compiler",
    value = "COMPILER",
    allowed = {
        { "gcc", "GNU GCC" },
        { "clang", "Clang" },
        { "msvc", "MSVC" }
    }
}

newoption {
    trigger = "ci",
    description = "Build all PAL systems for CI"
}

workspace(workspaceName)
    if PAL_BUILD_TEST_APPLICATION then
        startproject("tests")
    else
        startproject("abi-dump")
    end

    if PAL_BUILD_STATIC_LIBRARY then
        staticruntime "on"
    else
        staticruntime "off"
    end

    multiprocessorcompile "On"
    cdialect "C99"
    architecture "x64"
    language "C"

    configurations { "Debug", "Release" }

    filter {"system:windows", "configurations:*"}
        systemversion "latest"

    filter "configurations:Debug"
        symbols "on"
        runtime "Debug"

    filter "configurations:Release"
        symbols "off"
        runtime "Release"
        optimize "full"

    filter {}

    -- override
    if _OPTIONS["ci"] then
        PAL_BUILD_STATIC_LIBRARY = false
        PAL_BUILD_TEST_APPLICATION = true
        PAL_BUILD_ABI_DUMP = true
        PAL_BUILD_SYSTEM_MODULE = true
        PAL_BUILD_THREAD_MODULE = true
        PAL_BUILD_VIDEO_MODULE = true
        PAL_BUILD_OPENGL_MODULE = true
        PAL_BUILD_GRAPHICS_MODULE = true
    end

    if (_ACTION == "gmake") then
        if os.target() == "windows" then
            local gccPath = getCommandOutput("where gcc.exe 2>nul")
            local gccBinPath = path.getdirectory(gccPath)
            gccBasePath = path.getdirectory(gccBinPath)
            debuggerPath = getCommandOutput("where gdb.exe 2>nul")

            if (_OPTIONS["compiler"] == "clang") then
                toolset("clang")
    
                buildoptions {
                    "-target x86_64-w64-windows-gnu",
                    "-I" .. gccBasePath .. "/include",
                    "-I" .. gccBasePath .. "/ucrt/include",
                    "-I" .. gccBasePath .. "/mingw/include"
                }
    
                linkoptions {
                    "-target x86_64-w64-windows-gnu",
                    "-L" .. gccBasePath .. "/lib",
                    "-L" .. gccBasePath .. "/mingw/lib"
                }

                intellisenseMode = "windows-clang-x64"
                compilerPath = getCommandOutput("where clang.exe 2>nul")
            else
                -- GCC
                intellisenseMode = "gcc-x64"
                compilerPath = gccPath
            end
        else
            -- linux
            if (_OPTIONS["compiler"] == "clang") then
                toolset("clang")

                intellisenseMode = "linux-clang-x64"
                compilerPath = "/usr/bin/clang"
            else
                -- GCC
                intellisenseMode = "linux-gcc-x64"
                compilerPath = "/usr/bin/gcc"
            end

            debuggerPath = "/usr/bin/gdb"
        end

        -- Warnings for both linux and windows
        buildoptions {
            "-Wno-switch",         -- for switch statements
            "-Wno-switch-enum",    -- for switch statements
            "-Wall",
            "-Wextra",
            "-Wpedantic",
            "-Wconversion",
            "-Wsign-conversion",
            "-Werror"
        }
    end

    if (_ACTION == "vs2022") or (_ACTION == "vs2026") then
        if (_OPTIONS["compiler"] == "clang") then
            toolset("clang")
        end

        defines {
            "_CRT_SECURE_NO_WARNINGS"
        }

        buildoptions {
            "/W4",
            "-WX"
        }

        disablewarnings {
            "6387",
            "4018",
            "4133",
            "4101"
        }
    end

    if (PAL_BUILD_SYSTEM_MODULE) then
        defines { "PAL_HAS_SYSTEM_MODULE=1" }
    else
        defines { "PAL_HAS_SYSTEM_MODULE=0" }
    end

    if (PAL_BUILD_THREAD_MODULE) then
        defines { "PAL_HAS_THREAD_MODULE=1" }
    else
        defines { "PAL_HAS_THREAD_MODULE=0" }
    end

    if (PAL_BUILD_VIDEO_MODULE) then
        defines { "PAL_HAS_VIDEO_MODULE=1" }
    else
        defines { "PAL_HAS_VIDEO_MODULE=0" }
    end

    if (PAL_BUILD_OPENGL_MODULE) then
        defines { "PAL_HAS_OPENGL_MODULE=1" }
    else
        defines { "PAL_HAS_OPENGL_MODULE=0" }
    end

    if (PAL_BUILD_GRAPHICS_MODULE) then
        defines { "PAL_HAS_GRAPHICS_MODULE=1" }
    else
        defines { "PAL_HAS_GRAPHICS_MODULE=0" }
    end

    if (PAL_BUILD_TEST_APPLICATION) then
        include "tests/tests.lua"
    end

    if (PAL_BUILD_ABI_DUMP) then
        include "tools/abi_dump/abi_dump.lua"
    end

    include "pal.lua"