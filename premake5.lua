
dofile("pal_config.lua")

target_dir = "%{wks.location}/bin/%{cfg.buildcfg}"
obj_dir = "%{wks.location}/build"

workspace "PAL Workspace"
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

    if (PAL_BUILD_EXAMPLES) then
        if (PAL_BUILD_VIDEO) then
            include "examples/video_window/video_window.lua"
        end

        if (PAL_BUILD_OPENGL and PAL_BUILD_VIDEO) then
            include "examples/opengl_window/opengl_window.lua"
        end
    end

    include "pal.lua"
    