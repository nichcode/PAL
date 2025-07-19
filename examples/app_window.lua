
project "AppWindow"
    language "C"
    kind "ConsoleApp"

    targetdir(target_dir)
    objdir(obj_dir) 

    files { "src/app_window.c" }
    includedirs { "%{wks.location}/include" }
    links { "PAL" }