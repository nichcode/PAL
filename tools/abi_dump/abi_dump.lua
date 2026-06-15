
project "pal-abi-dump"
    kind "ConsoleApp"

    targetdir(targetDir)
    objdir(objDir)

    files {
        "core_abi_dump.c",
        "abi_dump_main.c"
    }

    includedirs { 
        "%{wks.location}/include",
        "%{wks.location}/tools/abi_dump" 
    }
    
    links { "PAL" }
