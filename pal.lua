
project "Pal"
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

    files { 
        "src/core/pal_memory.c", 
        "src/core/pal_result.c", 
        "src/core/pal_version.c",
        "src/core/pal_format.c",
        "src/core/pal_log.c"
    }

    filter {"system:windows", "configurations:*"}
        files { 
            "src/core/win32/**.c",
            "src/platform/win32/**.c",
        }
    filter {}

    if (PAL_BUILD_VIDEO) then
        files { "src/video/pal_video.c" }    
        filter {"system:windows", "configurations:*"}
            files { "src/video/win32/**.c" }
        filter {}
    end

    if (PAL_BUILD_EVENT) then
        files { 
            "src/event/pal_event.c", 
            "src/event/pal_event_queue.c",
            "src/event/pal_window_event.c"
        }
    end

    if (PAL_BUILD_INPUT) then
        files { 
            "src/input/pal_input.c"
        }
        filter {"system:windows", "configurations:*"}
            files { "src/input/win32/**.c" }
        filter {}
    end

    includedirs {
        "include",
        "src"
    }