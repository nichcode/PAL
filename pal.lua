
project "PAL2"
    if PAL_BUILD_STATIC then
        kind "StaticLib"
    else
        kind "SharedLib"
        defines {
            "_PAL_EXPORT",
            "_PAL_BUILD_DLL"
        }
    end

    targetdir(targetDir)
    objdir(objDir)
    includedirs { "include", "src" }

    files { "src/core/*.c" }
    filter {"system:windows", "configurations:*"}
         files { "src/core/win32/*.c" }

    filter {"system:windows", "configurations:*"}
        files { "src/core/posix/*.c" }
    
    filter { }
    