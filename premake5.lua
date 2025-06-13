
newoption {
    trigger = "static",
    description = "build PAL as a static library(.lib or .a)"
}

newoption {
    trigger = "tests",
    description = "build PAL with tests as a single application"
}

target_dir = "%{wks.location}/bin"
obj_dir = "%{wks.location}/build"

workspace "PAL"
if (_OPTIONS["tests"]) then
    startproject "tests"
end

if (_OPTIONS["static"]) then
    staticruntime "on"
else
    staticruntime "off"
end

    configurations { "Debug", "Release" }
    flags { "MultiProcessorCompile" }
   
    defines { "UNICODE", "_UNICODE" }

    filter {"system:windows", "configurations:*"}
        architecture "x64"
        systemversion "latest"
        cppdialect "C++17"

    filter "configurations:Debug"
        symbols "on"
        runtime "Debug"
        targetsuffix "_d"

        defines { "PAL_CONFIG_DEBUG" }

    filter "configurations:Release"
        symbols "off"
        runtime "Release"
        optimize "full"

    -- PAL
    project "PAL"
        language "C++"

        if (_OPTIONS["static"]) then
            kind "StaticLib"
        else
            kind "SharedLib"
            defines { "PAL_EXPORT" }
        end

        targetdir(target_dir)
        objdir(obj_dir)

        pchheader "src/PAL_pch.h"
        files { "src/PAL_build.cpp" }

        includedirs {
            "include",
            "src"
        }

if (_OPTIONS["tests"]) then
    include "tests/tests.lua"
end