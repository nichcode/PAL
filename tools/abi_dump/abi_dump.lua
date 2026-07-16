
project "abi-dump"
    kind "ConsoleApp"

    targetdir(targetDir)
    objdir(objDir)

    files {
        "core_abi_dump.c",
        "event_abi_dump.c",
        "thread_abi_dump.c",
        "system_abi_dump.c",
        "video_abi_dump.c",
        "opengl_abi_dump.c",
        -- "graphics_abi_dump.c",
        "abi_dump_main.c"
    }

    includedirs { 
        "%{wks.location}/include",
        "%{wks.location}/tools/abi_dump" 
    }
    
    links { "PAL" }
