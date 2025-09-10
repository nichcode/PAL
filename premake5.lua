
dofile("pal_config.lua")

target_dir = "%{wks.location}/bin/%{cfg.buildcfg}"
obj_dir = "%{wks.location}/build"

workspace "PAL_workspace"
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

    filter {"system:windows", "configurations:*"}
        architecture "x64"
        systemversion "latest"
        cdialect "C99"

    filter "configurations:Debug"
        symbols "on"
        runtime "Debug"

    filter "configurations:Release"
        symbols "off"
        runtime "Release"
        optimize "full"

    filter {}

    if (PAL_BUILD_TESTS) then
        include "tests/tests.lua"
    end

    if (PAL_BUILD_STRESS_TESTS) then
        include "stress_test/stress_test.lua"
    end
    
    include "pal.lua"
    