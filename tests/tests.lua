
project "Tests"
    language "C"
    kind "ConsoleApp"

    targetdir(target_dir)
    objdir(obj_dir)

    files { 
        "src/main.c",

        "src/allocator_test.c"
    }

    includedirs { "%{wks.location}/include" }
    links { "PAL" }