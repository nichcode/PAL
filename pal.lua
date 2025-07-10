
project "pal"
    language "C"

    if PAL_BUILD_STATIC then
        kind "StaticLib"
    else
        kind "SharedLib"
        defines { "PAL_EXPORT" }
    end

    targetdir(target_dir)
    objdir(obj_dir)

    pchheader "src/pal_pch.h"
    files {
        "src/core/pal_version.c",
        "src/core/pal_memory.c",
        "src/core/pal_error.c",
        "src/core/pal_log.c",
        "src/core/pal_format.c"
    }

    filter {"system:windows", "configurations:*"}
        files {
            "src/core/pal_win32tls.c"
        }

    filter {}

    -- video system
    if (PAL_BUILD_VIDEO) then
        files { 
            "src/video/pal_video.c",
        }

        filter {"system:windows", "configurations:*"}
            files {
                "src/video/pal_win32display.c",
                "src/video/pal_win32window.c"
            }

        filter {}
    end

    includedirs {
        "include",
        "src"
    }