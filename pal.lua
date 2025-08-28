
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

    filter {"system:windows", "configurations:*"}
        files { "src/core/pal_core_win32.c" }
    filter {}

    includedirs {
        "include",
        "src"
    }