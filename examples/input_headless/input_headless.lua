
project "input_headless"
    language "C"
    kind "ConsoleApp"

    targetdir(target_dir)
    objdir(obj_dir)

    files { "input_headless.c" }
    includedirs { "%{wks.location}/include" }
    links { "PAL" }