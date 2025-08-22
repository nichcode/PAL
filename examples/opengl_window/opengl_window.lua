
project "opengl_window"
    language "C"
    kind "ConsoleApp"

    targetdir(target_dir)
    objdir(obj_dir)

    files { 
        "opengl_window.c",
        "opengl_helper.c",

        "%{wks.location}/examples/glad/src/glad.c"
    }

    includedirs { 
        "%{wks.location}/include",
        "%{wks.location}/examples/glad/include"
    }

    links { "PAL" }