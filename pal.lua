
function writeConfig(path)
    local file = io.open(path, "w")
    file:write("\n// Auto Generated Config Header From pal_config.lua\n")
    file:write("// Must not be edited manually\n\n")

    if (PAL_BUILD_SYSTEM) then
        file:write("#define PAL_HAS_SYSTEM 1\n")
    else
        file:write("#define PAL_HAS_SYSTEM 0\n")
    end

    if (PAL_BUILD_THREAD) then
        file:write("#define PAL_HAS_THREAD 1\n")
    else
        file:write("#define PAL_HAS_THREAD 0\n")
    end

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

    includedirs {
        "include",
        "src"
    }

    files { 
        "src/pal_core.c",
        "src/pal_event.c"
    }

    if (PAL_BUILD_SYSTEM) then
        filter {"system:windows", "configurations:*"}
        files { "src/system/pal_system_win32.c" }
        filter {}
    end

    if (PAL_BUILD_THREAD) then
        filter {"system:windows", "configurations:*"}
        files { "src/thread/pal_thread_win32.c" }
        filter {}
    end

    if (PAL_BUILD_VIDEO) then
        filter {"system:windows", "configurations:*"}
        files { "src/video/pal_video_win32.c" }
        filter {}
    end

    writeConfig("include/pal/pal_config.h")