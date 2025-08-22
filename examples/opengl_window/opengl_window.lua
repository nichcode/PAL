
project "OpenglWindow"
    language "C"
    kind "ConsoleApp"

    targetdir(target_dir)
    objdir(obj_dir)

    if (PAL_BUILD_OPENGL and PAL_BUILD_VIDEO) then
        files { 
            "opengl_window.c",
            "opengl_helper.c",

            "%{wks.location}/examples/glad/src/glad.c"
        }
    end

    includedirs { 
        "%{wks.location}/include",
        "%{wks.location}/examples/glad/include"
    }

    links { "PAL" }