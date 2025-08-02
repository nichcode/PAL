
function writeConfig(path)
    local file = io.open(path, "w")
    file:write("\n// Auto Generated Config Header\n")
    file:write("// Must not be edited manually\n\n")

    if (PAL_BUILD_VIDEO) then
        file:write("#define PAL_HAS_VIDEO 1\n")
    else
        file:write("#define PAL_HAS_VIDEO 0\n")
    end
    
    file:close()
end

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
        "src/core/pal_memory.c"
    }

    filter {"system:windows", "configurations:*"}
    files { 
        "src/core/win32/**.c"
    }

    filter {}

    if (PAL_BUILD_VIDEO) then
        defines { "_PAL_BUILD_VIDEO" }

        filter {"system:windows", "configurations:*"}
        files {
            "src/video/win32/**.c"
        }
        filter {}
    end

    writeConfig("include/pal/pal_config.h")