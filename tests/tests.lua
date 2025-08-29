
project "tests"
    language "C"
    kind "ConsoleApp"

    targetdir(target_dir)
    objdir(obj_dir)

    files { 
        "tests_main.c",
        "tests.c"
    }

    if (PAL_BUILD_SYSTEM) then
        files { 
            "system_test.c"
        }
    end

    includedirs { "%{wks.location}/include" }
    links { "PAL" }