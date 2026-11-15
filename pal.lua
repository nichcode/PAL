
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

    if (PAL_BUILD_VIDEO_MODULE) then
        files { "src/video/*.c" }

        if (os.target() == "linux") then
            -- check for wayland support. This is cross compiler
            local waylandPaths = {
                "/usr/include/wayland-client.h",
                "/usr/include/x86_64-linux-gnu/wayland-client.h"
            }

            local found = false
            for _, path in ipairs(waylandPaths) do
                local file = io.open(path, "r")
                if file then
                    file:close()
                    found = true
                    break
                end
            end

            if found then
                defines { "PAL_HAS_WAYLAND_BACKEND=1" }
            else
                defines { "PAL_HAS_WAYLAND_BACKEND=0" }
            end

            -- check for Xlib support. This is cross compiler
            local XPaths = {
                "/usr/include/X11/Xlib.h",
                "/usr/include/x86_64-linux-gnu/X11/Xlib.h"
            }

            -- Remove X11 until wayland is done
            -- found = false
            -- for _, path in ipairs(XPaths) do
            --     local file = io.open(path, "r")
            --     if file then
            --         file:close()
            --         found = true
            --         break
            --     end
            -- end

            -- if found then
            --     defines { "PAL_HAS_X11_BACKEND=1" }
            -- else
            --     defines { "PAL_HAS_X11_BACKEND=0" }
            -- end
        end

        filter {"system:windows", "configurations:*"}
            files { "src/video/win32/*.c" }

        filter {"system:linux", "configurations:*"}
            files {
                "src/video/x11/*.c",
                "src/video/wayland/*.c"
            }
            
        filter {}
    end