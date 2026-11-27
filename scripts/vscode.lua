local function getCommandOutput(cmd)
    local result, _ = os.outputof(cmd)
    if result then
        result = result:gsub("[\r\n%s]+$", "")
        if result ~= "" then
            return result:gsub("\\", "/")
        end
    end
    return nil
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

function generateProperties()
    print("\n=======================================================")
    print("Generating .vscode/c_cpp_properties.json")

    local prjDefines = {}
    local prjIncludes = {}
    local workspace = premake.global.getWorkspace(workspaceName)

    for prj in premake.workspace.eachproject(workspace) do
        for _, dir in ipairs(prj.includedirs or {}) do
            table.insert(prjIncludes, dir)
        end

        for _, define in ipairs(prj.defines or {}) do
            table.insert(prjDefines, define)
        end
    end

    prjIncludes = removeDuplicates(prjIncludes)
    prjDefines = removeDuplicates(prjDefines)

    local file = io.open(".vscode/c_cpp_properties.json", "w")
    if not file then
        print("Failed to write to .vscode/c_cpp_properties.json")
        return
    end

    file:write('{\n')
    file:write('    "configurations": [\n')
    file:write('        {\n')
    file:write(string.format('            "name": "%s",\n', workspaceName))

    file:write('            "includePath": [\n')
    for i, dir in ipairs(prjIncludes) do
        file:write(string.format('                "%s"%s\n', dir, i < #prjIncludes and "," or ""))
    end
    file:write('            ],\n')

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

function generateTasks()
    print("\n=======================================================")
    print("Generating .vscode/tasks.json")

    local file = io.open(".vscode/tasks.json", "w")
    if not file then
        print("Failed to write to .vscode/tasks.json")
        return
    end

    local cmd = "make -j config="

    file:write('{\n')
    file:write('    "inputs": [\n')
    file:write('        {\n')
    file:write('            "id": "option",\n')
    file:write('            "type": "pickString",\n')
    file:write('            "description": "select option",\n')
    file:write('            "options": [\n')
    file:write('                "debug",\n')
    file:write('                "release"\n')
    file:write('            ]\n')
    file:write('        }\n')
    file:write('    ],\n')

    file:write('    "tasks": [\n')
    file:write('        {\n')
    file:write('            "type": "shell",\n')
    file:write(string.format('            "label": "%s",\n', workspaceName))
    file:write(string.format('            "command": "%s%s",\n', cmd, "${input:option}"))
    file:write('            "problemMatcher": [\n')
    file:write('                "$gcc"\n')
    file:write('            ],\n')
    file:write('            "group": {\n')
    file:write('                "kind": "build",\n')
    file:write('                "isDefault": true\n')
    file:write('            }\n')
    file:write('        }\n')
    file:write('    ],\n')
    file:write('    "version": "2.0.0"\n')
    file:write('}\n')
    file:close()
end

function generateLaunch()
    print("\n=======================================================")
    print("Generating .vscode/launch.json")

    local file = io.open(".vscode/launch.json", "w")
    if not file then
        print("Failed to write to .vscode/launch.json")
        return
    end

    local projects = {}
    local workspace = premake.global.getWorkspace(workspaceName)
    for prj in premake.workspace.eachproject(workspace) do
        if prj.kind == "ConsoleApp" then
            table.insert(projects, prj)
        end
    end

    local extension = ""
    local gdbPath = "/usr/bin/gdb"
    if os.target() == "windows" then
        extension = ".exe"
        gdbPath = getCommandOutput("where gdb.exe 2>nul") or "gdb.exe"
    end

    file:write('{\n')
    file:write('    "inputs": [\n')
    file:write('        {\n')
    file:write('            "id": "program",\n')
    file:write('            "type": "pickString",\n')
    file:write('            "description": "select program",\n')
    file:write('            "options": [\n')

    for prjI, prj in ipairs(projects) do
        file:write(string.format('                "%s"%s\n', prj.name, prjI < #projects and "," or ""))
    end

    file:write('            ]\n')
    file:write('        },\n')
    file:write('        {\n')
    file:write('            "id": "config",\n')
    file:write('            "type": "pickString",\n')
    file:write('            "description": "select configuration",\n')
    file:write('            "options": [\n')
    file:write('                "debug",\n')
    file:write('                "release"\n')
    file:write('            ]\n')
    file:write('        }\n')
    file:write('    ],\n')

    file:write('    "configurations": [\n')
    file:write('        {\n')
    file:write('            "name": "${input:program}",\n')
    file:write('            "type": "cppdbg",\n')
    file:write('            "request": "launch",\n')
    file:write('            "stopAtEntry": false,\n')
    file:write('            "cwd": "${workspaceFolder}",\n')
    file:write('            "environment": [],\n')
    file:write('            "externalConsole": false,\n')
    file:write(string.format('            "program": "${workspaceFolder}/bin/${input:config}/${input:program}%s",\n', extension))
    file:write('            "MIMode": "gdb",\n')
    file:write(string.format('            "miDebuggerPath": "%s",\n', gdbPath))
    file:write('            "setupCommands": [\n')
    file:write('                {\n')
    file:write('                    "description": "Let gdb load our script",\n')
    file:write('                    "text": "source ${workspaceFolder}/build/gdb-format.py",\n')
    file:write('                    "ignoreFailures": true\n')
    file:write('                },\n')
    file:write('                {\n')
    file:write('                    "description": "Enable pretty printing for gdb",\n')
    file:write('                    "text": "-enable-pretty-printing",\n')
    file:write('                    "ignoreFailures": false\n')
    file:write('                },\n')
    file:write('                {\n')
    file:write('                    "description": "Set disassembly flavor to intel",\n')
    file:write('                    "text": "-gdb-set disassembly-flavor intel",\n')
    file:write('                    "ignoreFailures": false\n')
    file:write('                }\n')
    file:write('            ]\n')
    file:write('        }\n')
    file:write('    ],\n')
    file:write('    "version": "0.2.0"\n')
    file:write('}\n')
    file:close()
end

function generateSettings()
    print("\n=======================================================")
    print("Generating .vscode/settings.json")

    local file = io.open(".vscode/settings.json", "w")
    if not file then
        print("Failed to write to .vscode/settings.json")
        return
    end

    file:write('{\n')
    file:write('    "files.associations": {\n')
    file:write('        ".clang-tidy": "yaml",\n')
    file:write('        ".clang-format": "yaml"\n')
    file:write('    }\n')
    file:write('}\n')
    file:close()
end

function generateGdbFormatter()
    print("\n=======================================================")
    print("Generating GDB formatter")

    local nullDev = os.ishost("windows") and "NUL" or "/dev/null"
    local candidates = { "python", "python3", "py -3" }

    for _, py in ipairs(candidates) do
        local cmd = string.format(
            '%s scripts/gen-formatdb.py >%s 2>&1 && %s scripts/gen-format.py gdb >%s 2>&1',
            py, nullDev, py, nullDev
        )
        
        if os.execute(cmd) then
            return
        end
    end

    print("Python not found. Skipping GDB formatter generation.")
end


function generateVSCode()
    os.mkdir(".vscode")
    
    generateProperties()
    generateTasks()
    generateLaunch()
    generateSettings()
    generateGdbFormatter()
end