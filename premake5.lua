dofile("pal_config.lua")
dofile("scripts/vscode.lua")
dofile("scripts/commands.lua")

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
    trigger = "override",
    description = "Override the build options with the default"
}

targetDir = "%{wks.location}/bin/%{cfg.buildcfg}"
objDir = "%{wks.location}/build"

workspaceName = "PALWorkspace"
compilerPath = ""
intellisenseMode = ""
gccBasePath = ""
mingwFlags = {}

-- Windows binaries can be installed in custom locations on a user's system.
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

premake.override(premake.action, "call", function(base, action)
    base(action)

    if action == "gmake" and PAL_GENERATE_VSCODE then
        generateVSCode()
    end

    if PAL_GENERATE_COMPILE_COMMANDS then
        generateCompileCommands()
    end

    if PAL_BUILD_TESTS then
        generateTests()
    end
end)

workspace(workspaceName)
    multiprocessorcompile "On"
    cdialect "C99"
    architecture "x64"
    language "C"

    configurations { "debug", "release" }

    filter { "system:windows", "configurations:*" }
        systemversion "latest"

    filter "configurations:debug"
        symbols "on"
        runtime "Debug"
        defines { "DEBUG" }

    filter "configurations:release"
        symbols "off"
        runtime "Release"
        optimize "full"
        defines { "NDEBUG" }
        linktimeoptimization "On"

    filter { "configurations:release", "toolset:gcc or clang" }
        buildoptions { "-fvisibility=hidden" }

    filter {}

    if _ACTION == "gmake" then
        if os.target() == "windows" then
            local gccPath = getCommandOutput("where gcc.exe 2>nul")
            if gccPath then
                local gccBinPath = path.getdirectory(gccPath)
                gccBasePath = path.getdirectory(gccBinPath)
            end

            if _OPTIONS["compiler"] == "clang" then
                toolset("clang")

                mingwFlags = { "-target x86_64-w64-windows-gnu" }
                if gccBasePath ~= "" then
                    table.insert(mingwFlags, "-I" .. gccBasePath .. "/include")
                    table.insert(mingwFlags, "-I" .. gccBasePath .. "/ucrt/include")
                    table.insert(mingwFlags, "-I" .. gccBasePath .. "/mingw/include")
                end

                buildoptions(mingwFlags)

                if gccBasePath ~= "" then
                    linkoptions {
                        "-target x86_64-w64-windows-gnu",
                        "-L" .. gccBasePath .. "/lib",
                        "-L" .. gccBasePath .. "/mingw/lib"
                    }
                end

                intellisenseMode = "windows-clang-x64"
                compilerPath = getCommandOutput("where clang.exe 2>nul") or ""
            else
                intellisenseMode = "gcc-x64"
                compilerPath = gccPath or ""
            end
        else
            -- Linux
            if _OPTIONS["compiler"] == "clang" then
                toolset("clang")
                intellisenseMode = "linux-clang-x64"
                compilerPath = "/usr/bin/clang"
            else
                intellisenseMode = "linux-gcc-x64"
                compilerPath = "/usr/bin/gcc"
            end
        end

        buildoptions {
            "-Wno-switch",
            "-Wno-switch-enum",
            "-Wall",
            "-Wextra",
            "-Wpedantic",
            "-Wconversion",
            "-Wsign-conversion",
            "-Werror",
            "-Wno-cast-function-type"
        }
    end

    if _ACTION == "vs2022" or _ACTION == "vs2026" then
        if _OPTIONS["compiler"] == "clang" then
            toolset("clang")
        end

        defines { "_CRT_SECURE_NO_WARNINGS" }

        buildoptions {
            "/W4",
            "/WX"
        }

        disablewarnings {
            "6387",
            "4018",
            "4133",
            "4101"
        }
    end

    include "pal.lua"

    staticruntime(PAL_BUILD_STATIC and "on" or "off")
    defines(getDefines())

    if PAL_BUILD_TESTS then
        include "tests/tests.lua"
    end

    if PAL_BUILD_ABI_DUMP then
        include "tools/abi-dump/abi-dump.lua"
    end