
project "OpenglWindow"
    language "C"
    kind "ConsoleApp"

    targetdir(target_dir)
    objdir(obj_dir)

    if (PAL_BUILD_OPENGL and PAL_BUILD_VIDEO) then
        files { 
            "src/opengl_window.c",
            "glad/src/glad.c"
        }
    end

    includedirs { 
        "%{wks.location}/include",
        "glad/include"
    }

    links { "PAL" }