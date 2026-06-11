
dofile("pal_config.lua")

targetDir = "%{wks.location}/bin/%{cfg.buildcfg}"
objDir = "%{wks.location}/build"

workspaceName = "PALWorkspace"
compilerPath = ""
intellisenseMode = ""
problemMatcher = ""
buildCommand = ""
cleanCommand = ""

debugConfiguration = ""
releaseConfiguration = ""
debuggerPath = ""

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

    -- write to file
    os.execute("mkdir .vscode 2>nul") -- ensure .vscode directory exists
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
    local configuration = ""
    local isDefault = "false"
    local command = ""

    if actionType == "buildDebug" then
        name = "build debug"
        configuration = debugConfiguration
        command = buildCommand
        isDefault = "true"

    elseif actionType == "buildRelease" then
        name = "build release"
        configuration = releaseConfiguration
        command = buildCommand

    elseif actionType == "cleanDebug" then
        name = "clean debug"
        configuration = debugConfiguration
        command = cleanCommand

    elseif actionType == "cleanRelease" then
        name = "clean release"
        configuration = releaseConfiguration
        command = cleanCommand
    end
    
    file:write("        {\n")
    file:write('            "type": "shell",\n')
    file:write(string.format('            "label": "%s %s",\n', workspaceName, name))
    file:write(string.format('            "command": "%s %s",\n', command, configuration))

    file:write('            "options": {\n')
    file:write('                "cwd": "${workspaceFolder}"\n')
    file:write('            },\n')

    file:write('            "problemMatcher": [\n')
    file:write(string.format('                "$%s",\n', problemMatcher))
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

local function writeLaunchConfiguration(file, isDebug)
    local name = ""
    local launchType = ""
    local preLaunchTask = ""

    if isDebug then
        name = "launch debug"
        launchType = "cppdbg"
        preLaunchTask = "build debug"

    else
        name = "launch release"
        launchType = "cppvsdbg"
        preLaunchTask = "build release"
    end

    file:write("        {\n")
    file:write(string.format('            "name": "%s %s",\n', workspaceName, name))
    file:write(string.format('            "type": "%s",\n', launchType))
    file:write('            "request": "launch",\n')
    file:write('            "stopAtEntry": false,\n')
    file:write('            "cwd": "${workspaceFolder}/tests",\n')

    file:write('            "environment": [],\n')
    file:write('            "externalConsole": false,\n')
    file:write(string.format('            "preLaunchTask": "%s %s",\n', workspaceName, preLaunchTask))

    if isDebug then
        if os.target() == "windows" then
            file:write('            "program": "${workspaceFolder}/bin/Debug/tests.exe",\n')
        else
            file:write('            "program": "${workspaceFolder}/bin/Debug/tests",\n')
        end

    else
        if os.target() == "windows" then
            file:write('            "program": "${workspaceFolder}/bin/Release/tests.exe",\n')
        else
            file:write('            "program": "${workspaceFolder}/bin/Release/tests",\n')
        end
    end

    if launchType == "cppdbg" then
        file:write('            "MIMode": "gdb",\n')
        file:write(string.format('            "miDebuggerPath": "%s",\n', debuggerPath))
    end

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

        writeLaunchConfiguration(file, true)
        file:write("        },\n")
        file:write('\n')

        writeLaunchConfiguration(file, false)
        file:write("        }\n")

        file:write("    ],\n")
        file:write('    "version": "0.2.0"\n')
        file:write("}\n")
        file:close()
    end
end

-- generate vscode properties
premake.override(premake.action, "call", function(base, action)
    base(action)
    generateVscodeProperties()
    generateTasksJson()
    generateLaunchJson()
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

workspace(workspaceName)
    if PAL_BUILD_TESTS then
        startproject("tests")
    end

    if PAL_BUILD_STATIC then
        staticruntime "on"
    else
        staticruntime "off"
    end

    multiprocessorcompile "On"
    configurations { "Debug", "Release" }

    filter {"system:windows", "configurations:*"}
        architecture "x64"
        systemversion "latest"
        cdialect "C99"

    filter {"system:linux", "configurations:*"}
        architecture "x86_64"
        cdialect "C99"

    filter "configurations:Debug"
        symbols "on"
        runtime "Debug"

    filter "configurations:Release"
        symbols "off"
        runtime "Release"
        optimize "full"

    filter {}

    if (_ACTION == "gmake") then
        problemMatcher = "gcc"
        command = "make all"
        buildCommand = "make all"
        cleanCommand = "make clean"

        debugConfiguration = "config=debug"
        releaseConfiguration = "config=release"

        if os.target() == "windows" then
            local gccPath = getCommandOutput("where gcc.exe 2>nul")
            local gccBinPath = path.getdirectory(gccPath)
            local gccBasePath = path.getdirectory(gccBinPath)
            debuggerPath = getCommandOutput("where gdb.exe 2>nul")

            if (_OPTIONS["compiler"] == "clang") then
                toolset("clang")
    
                buildoptions {
                    "-target x86_64-w64-windows-gnu",
                    "-I" .. gccBasePath .. "/include",
                    "-I" .. gccBasePath .. "/ucrt/include",
                    "-I" .. gccBasePath .. "/mingw/include",
    
                    -- warnings
                    "-Wno-switch",        -- for switch statements
                    "-Wno-switch-enum"    -- for switch statements
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
        end
    end

    if (_ACTION == "vs2022") or (_ACTION == "vs2026") then
        problemMatcher = "msCompile"
        command = "msbuild"
        cleanCommand = "msbuild /t:clean"

        debugConfiguration = "p:Configuration=Debug"
        releaseConfiguration = "p:Configuration=Release"

        if (_OPTIONS["compiler"] == "clang") then
            toolset("clang")

            intellisenseMode = "windows-clang-x64"
            compilerPath = getCommandOutput("where clang.exe 2>nul")
        else
            -- MSVC
            intellisenseMode = "windows-msvc-x64"
            compilerPath = getCommandOutput("where cl.exe 2>nul")
        end

        defines {
            "_CRT_SECURE_NO_WARNINGS"
        }

        disablewarnings {
            "6387",
            "4018",
            "4133",
            "4101"
        }
    end

    if (PAL_BUILD_TESTS) then
        include "tests/tests.lua"
    end

    include "pal.lua"
   