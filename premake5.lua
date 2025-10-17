
dofile("pal_config.lua")

target_dir = "%{wks.location}/bin/%{cfg.buildcfg}"
obj_dir = "%{wks.location}/build"

local ucrt = os.getenv("UCRT64") or "C:/msys64/ucrt64"

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

workspace "PAL_workspace"
    if PAL_BUILD_TESTS then
        startproject("tests")
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

    if (_ACTION == "gmake2") then
        if (_OPTIONS["compiler"] == "clang") then
            toolset("clang")

            buildoptions {
                "-target x86_64-w64-windows-gnu",
                "-I" .. ucrt .. "/include",
                "-I" .. ucrt .. "/ucrt/include",
                "-I" .. ucrt .. "/mingw/include",
    
                -- warnings
                "-Wno-switch",        -- for switch statements
                "-Wno-switch-enum"    -- for switch statements
            }

            linkoptions {
                "-target x86_64-w64-windows-gnu",  
                "-L" .. ucrt .. "/lib",
                "-L" .. ucrt .. "/mingw/lib"
            }
        end
    end

    if (_ACTION == "vs2022") then
        if (_OPTIONS["compiler"] == "clang") then  
            toolset("clang")
        end

        defines {
            "_CRT_SECURE_NO_WARNINGS"
        }
        disablewarnings {
            "6387"
        }
    end

    if (PAL_BUILD_TESTS) then
        include "tests/tests.lua"
    end

    include "pal.lua"
    