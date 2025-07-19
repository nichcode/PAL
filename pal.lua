
project "Pal"
    language "C"

    if (_OPTIONS["with-static"]) then
        kind "StaticLib"
    else
        kind "SharedLib"
        defines { "PAL_EXPORT" }
    end

    targetdir(target_dir)
    objdir(obj_dir)

    pchheader "src/pal_pch.h"

    files { 
        "src/core/pal_memory.c", 
        "src/core/pal_result.c", 
        "src/core/pal_version.c",
        "src/core/pal_format.c",
        "src/core/pal_log.c",
        "src/video/pal_video.c", 
        "src/event/pal_event.c", 
        "src/event/pal_event_queue.c"
    }

    filter {"system:windows", "configurations:*"}
        files { 
            "src/core/win32/**.c",
            "src/platform/win32/**.c",
            "src/video/win32/**.c"
        }
    filter {}

    includedirs {
        "include",
        "src"
    }