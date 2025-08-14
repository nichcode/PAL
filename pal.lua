
function writeConfig(path)
    local file = io.open(path, "w")
    file:write("\n// Auto Generated Config Header From pal_config.lua\n")
    file:write("// Must not be edited manually\n\n")

    if (PAL_BUILD_VIDEO) then
        file:write("#define PAL_HAS_VIDEO 1\n")
    else
        file:write("#define PAL_HAS_VIDEO 0\n")
    end

    if (PAL_BUILD_INPUT) then
        file:write("#define PAL_HAS_INPUT 1\n")
    else
        file:write("#define PAL_HAS_INPUT 0\n")
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

    files { "src/pal_core.c" }
    includedirs {
        "include",
        "src"
    }

    if (PAL_BUILD_VIDEO) then
        filter {"system:windows", "configurations:*"}
        files { "src/video/pal_video_win32.c" }
        filter {}
    end

    if (PAL_BUILD_INPUT) then
        filter {"system:windows", "configurations:*"}
        files { "src/input/pal_input_win32.c" }
        filter {}
    end

    writeConfig("include/pal/pal_config.h")