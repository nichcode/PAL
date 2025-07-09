
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

    includedirs {
        "include",
        "src"
    }