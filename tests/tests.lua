
project "tests"
    language "C"
    kind "ConsoleApp"

    targetdir(target_dir)
    objdir(obj_dir) 

    files { "main.c" }
    includedirs { "%{wks.location}/include" }
    links { "PAL" }