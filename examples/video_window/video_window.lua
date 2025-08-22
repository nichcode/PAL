
project "VideoWindow"
    language "C"
    kind "ConsoleApp"

    targetdir(target_dir)
    objdir(obj_dir)

    files { "video_window.c" }
    includedirs { "%{wks.location}/include" }
    links { "PAL" }