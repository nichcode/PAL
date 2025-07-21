
local gen = require("pal_generate_config")
dofile("pal_config.lua")

target_dir = "%{wks.location}/bin"
obj_dir = "%{wks.location}/build"

workspace "PAL"
    if PAL_BUILD_TESTS then
        startproject("Tests")
    end

    if PAL_BUILD_STATIC then
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
        cdialect "C99"

    filter "configurations:Debug"
        symbols "on"
        runtime "Debug"
        targetsuffix "_d"

    filter "configurations:Release"
        symbols "off"
        runtime "Release"
        optimize "full"

    filter {}

    if (PAL_BUILD_EXAMPLES) then
        if PAL_BUILD_VIDEO and PAL_BUILD_EVENT then
            include "examples/app_window.lua"
        end
    end

    if (PAL_BUILD_TESTS) then
        include "tests/tests.lua"
    end

    include "pal.lua"

    gen.writeConfigHeader("include/pal/pal_config.h")
    gen.writeProperties(".vscode/c_cpp_properties.json")    