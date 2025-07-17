
project "Pal"
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
        -- core
        "src/core/pal_memory.c", 
        "src/core/pal_result.c", 
        "src/core/pal_version.c",
        "src/core/pal_format.c",
        "src/core/pal_log.c"
    }

    filter {"system:windows", "configurations:*"}
        files { "src/core/win32/**.c" }
    filter {}

    includedirs {
        "include",
        "src"
    }