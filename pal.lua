if PAL_BUILD_STATIC == nil then
    dofile("pal_config.lua")
end

function applyOverrides()
    PAL_BUILD_STATIC = false
    PAL_BUILD_TESTS = true
    PAL_GENERATE_COMPILE_COMMANDS = true
    PAL_BUILD_ABI_DUMP = true
    PAL_BUILD_EVENT_MODULE = true
    PAL_BUILD_SYSTEM_MODULE = true
    PAL_BUILD_THREAD_MODULE = true
    PAL_BUILD_VIDEO_MODULE = true
    PAL_BUILD_OPENGL_MODULE = true
    PAL_BUILD_GRAPHICS_MODULE = true
end

if _OPTIONS and _OPTIONS["override"] then
    applyOverrides()
end

function getDefines()
    local defs = {
        "PAL_HAS_EVENT_MODULE=" .. (PAL_BUILD_EVENT_MODULE and "1" or "0"),
        "PAL_HAS_SYSTEM_MODULE=" .. (PAL_BUILD_SYSTEM_MODULE and "1" or "0"),
        "PAL_HAS_THREAD_MODULE=" .. (PAL_BUILD_THREAD_MODULE and "1" or "0"),
        "PAL_HAS_VIDEO_MODULE=" .. (PAL_BUILD_VIDEO_MODULE and "1" or "0"),
        "PAL_HAS_OPENGL_MODULE=" .. (PAL_BUILD_OPENGL_MODULE and "1" or "0"),
        "PAL_HAS_GRAPHICS_MODULE=" .. (PAL_BUILD_GRAPHICS_MODULE and "1" or "0")
    }

    if not PAL_BUILD_STATIC then
        table.insert(defs, "PAL_EXPORT")
    end

    return defs
end

project "PAL2"
    language "C"
    cdialect "C99"

    if PAL_BUILD_STATIC then
        kind "StaticLib"
    else
        kind "SharedLib"
        defines { "PAL_BUILD_DLL" }
    end

    defines(getDefines())

    targetdir(targetDir or "%{wks.location}/bin/%{cfg.buildcfg}")
    objdir(objDir or "%{wks.location}/build/%{prj.name}")

    includedirs { "include", "src" }
    files { "src/*.c", "src/core/*.c" }

    filter { "system:windows", "configurations:*" }
        files { "src/core/win32/*.c" }

    filter { "system:linux", "configurations:*" }
        files { "src/core/posix/*.c" }

    filter {}

    if PAL_BUILD_EVENT_MODULE then
        files { "src/event/*.c" }
    end

    if PAL_BUILD_SYSTEM_MODULE then
        files { "src/system/*.c" }

        filter { "system:windows", "configurations:*" }
            files { "src/system/win32/*.c" }

        filter { "system:linux", "configurations:*" }
            files { "src/system/linux/*.c" }

        filter {}
    end

    if PAL_BUILD_THREAD_MODULE then
        files { "src/thread/*.c" }

        filter { "system:windows", "configurations:*" }
            files { "src/thread/win32/*.c" }

        filter { "system:linux", "configurations:*" }
            files { "src/thread/posix/*.c" }

        filter {}
    end

    if PAL_BUILD_VIDEO_MODULE then
        files { "src/video/*.c" }

        if os.ishost("linux") then
            local waylandPaths = {
                "/usr/include/wayland-client.h",
                "/usr/include/x86_64-linux-gnu/wayland-client.h"
            }

            local hasWayland = false
            for _, headerPath in ipairs(waylandPaths) do
                if os.isfile(headerPath) then
                    hasWayland = true
                    break
                end
            end

            if hasWayland then
                defines { "PAL_HAS_WAYLAND_BACKEND=1" }
                files { "src/video/wayland/*.c" }
            else
                defines { "PAL_HAS_WAYLAND_BACKEND=0" }
            end

            local xPaths = {
                "/usr/include/X11/Xlib.h",
                "/usr/include/x86_64-linux-gnu/X11/Xlib.h"
            }

            local hasX11 = false
            for _, headerPath in ipairs(xPaths) do
                if os.isfile(headerPath) then
                    hasX11 = true
                    break
                end
            end

            if hasX11 then
                defines { "PAL_HAS_X11_BACKEND=1" }
                files { "src/video/x11/*.c" }
            else
                defines { "PAL_HAS_X11_BACKEND=0" }
            end
        end

        filter { "system:windows", "configurations:*" }
            files { "src/video/win32/*.c" }

        filter {}
    end

    if PAL_BUILD_OPENGL_MODULE then
        files { "src/opengl/*.c" }

        filter { "system:windows", "configurations:*" }
            files { "src/opengl/wgl/*.c" }

        filter { "system:linux", "configurations:*" }
            files { "src/opengl/egl/*.c" }

        filter {}
    end