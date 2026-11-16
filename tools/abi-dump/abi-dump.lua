
project "abi-dump"
    kind "ConsoleApp"

    targetdir(targetDir)
    objdir(objDir)

    files { "src/*.c" }

    if (PAL_BUILD_EVENT_MODULE) then
        files { "src/event/*.c" }
    end

    if (PAL_BUILD_SYSTEM_MODULE) then
        files { "src/system/*.c" }
    end

    if (PAL_BUILD_THREAD_MODULE) then
        files { "src/thread/*.c" }
    end

    if (PAL_BUILD_VIDEO_MODULE) then
        files { "src/video/*.c" }
    end

    if (PAL_BUILD_OPENGL_MODULE) then
        files { "src/opengl/*.c" }
    end

    if (PAL_BUILD_GRAPHICS_MODULE) then
        files { "src/graphics/*.c" }
    end


    includedirs { 
        "%{wks.location}/include", 
        "%{wks.location}/tools/abi-dump/src"
    }
    links { "PAL2" }
