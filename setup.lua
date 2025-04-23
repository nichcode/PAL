
newoption {
    trigger = "with-tests",
    description = "build PAL with test applications"
}

target_dir = "%{wks.location}/bin/%{cfg.buildcfg}"
obj_dir = "%{wks.location}/obj/%{cfg.buildcfg}/%{prj.name}"

workspace "PAL"
if (_OPTIONS["with-tests"]) then
    startproject "window_test"
end

    configurations { "Debug", "Release" }
    flags { "MultiProcessorCompile" }
    staticruntime "off"

    defines { "UNICODE", "_UNICODE" }

    filter {"system:windows", "configurations:*"}
        architecture "x64"
        systemversion "latest"
        cppdialect "C++17"

    filter "configurations:Debug"
        symbols "on"
        runtime "Debug"

        defines { "PAL_CONFIG_DEBUG" }

    filter "configurations:Release"
        symbols "off"
        runtime "Release"
        optimize "full"

if (_OPTIONS["with-tests"]) then
    include "tests/window_test.lua"
    include "tests/context_test.lua"
    include "tests/renderer_test.lua"
    include "tests/collision_test.lua"
end

    include "PAL.lua"