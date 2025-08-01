
project "PAL"
    language "C"

    if PAL_BUILD_STATIC then
        kind "StaticLib"
    else
        kind "SharedLib"
        defines { "_PAL_EXPORT" }
    end

    targetdir(target_dir)
    objdir(obj_dir)

    pchheader "src/pal_pch.h"
    includedirs {
        "include",
        "src"
    }

    files {
        "src/pal_result.c",
        
        -- core
        "src/core/pal_version.c",
    }

    filter {"system:windows", "configurations:*"}

    filter {}