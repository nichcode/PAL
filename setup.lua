
newoption {
    trigger = "with-shared",
    description = "build PAL as a shared library(.dll or .so)"
}

newoption {
    trigger = "with-tests",
    description = "build PAL with test applications"
}

target_dir = "%{wks.location}/bin"
obj_dir = "%{wks.location}/bin/obj/%{prj.name}"

workspace "PAL"
if (_OPTIONS["with-tests"]) then
    startproject "window_test"
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

if (_OPTIONS["with-tests"]) then
    include "tests/window_test.lua"
end
    include "PAL.lua"