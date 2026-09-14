
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
    files { "src/*.c", "src/core/*.c" }

    filter {"system:windows", "configurations:*"}
         files { "src/core/win32/*.c" }

    filter {"system:linux", "configurations:*"}
        files { "src/core/posix/*.c" }
    
    filter {}

    if (PAL_BUILD_EVENT_MODULE) then
        files { "src/event/*.c" }
    end

    if (PAL_BUILD_SYSTEM_MODULE) then
        files { "src/system/*.c" }

        filter {"system:windows", "configurations:*"}
            files { "src/system/win32/*.c" }

        filter {"system:linux", "configurations:*"}
            files { "src/system/linux/*.c" }

        filter {}
    end

    if (PAL_BUILD_THREAD_MODULE) then
        files { "src/thread/*.c" }

        filter {"system:windows", "configurations:*"}
            files { "src/thread/win32/*.c" }

        filter {"system:linux", "configurations:*"}
            files { "src/thread/posix/*.c" }

        filter {}
    end