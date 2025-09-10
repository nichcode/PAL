
project "stress_test"
    language "C"
    kind "ConsoleApp"

    targetdir(target_dir)
    objdir(obj_dir)

    files { 
        "stress_test_main.c",
        "stress_test.c"
    }

    includedirs { "%{wks.location}/include" }
    links { "PAL" }