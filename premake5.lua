
newoption {
    trigger = "with-tests",
    description = "build PAL with test applications"
}

newoption {
    trigger = "with-static",
    description = "build PAL as a static library"
}

newoption {
    trigger = "with-examples",
    description = "build PAL with example applications"
}

target_dir = "%{wks.location}/bin"
obj_dir = "%{wks.location}/build"

workspace "PAL"
    if (_OPTIONS["with-tests"]) then
        startproject("tests")
    end

    if (_OPTIONS["with-static"]) then
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

    if (_OPTIONS["with-tests"]) then
        include "tests/tests.lua"
    end

    if (_OPTIONS["with-examples"]) then
        include "examples/app_window.lua"
    end

    include "pal.lua"
    