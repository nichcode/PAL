
project "PAL"
    language "C"

    if PAL_BUILD_STATIC then
        kind "StaticLib"
    else
        kind "SharedLib"
        defines { "PAL_EXPORT" }
    end

    if PAL_BUILD_VIDEO then
        defines { "PAL_BUILD_VIDEO" }
    end

    targetdir(target_dir)
    objdir(obj_dir)
    pchheader "src/pal_pch.h"

    files {
        "src/containers/pal_hashmap.c",
        "src/core/pal_error.c",
        "src/core/pal_version.c",
    }

    filter {"system:windows", "configurations:*"}
        files {
            "src/core/pal_win32log.c",
            "src/core/pal_win32memory.c",
            "src/core/pal_win32tls.c",
        }

    if (PAL_BUILD_VIDEO) then
        files { "src/video/pal_video.c" }
        filter {"system:windows", "configurations:*"}
            files {
                "src/video/pal_win32display.c",
                "src/video/pal_win32window.c"
            }
    end

    includedirs {
        "include",
        "src"
    }