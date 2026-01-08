
dofile("pal_config.lua")

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

    if (PAL_BUILD_OPENGL) then
        file:write("#define PAL_HAS_OPENGL 1\n")
    else
        file:write("#define PAL_HAS_OPENGL 0\n")
    end

    if (PAL_BUILD_GRAPHICS) then
        file:write("#define PAL_HAS_GRAPHICS 1\n")
    else
        file:write("#define PAL_HAS_GRAPHICS 0\n")
    end

    file:close()
end

project "PAL"
    language "C"

    if PAL_BUILD_STATIC then
        kind "StaticLib"
    else
        kind "SharedLib"
        defines {
            "_PAL_EXPORT",
            "_PAL_BUILD_DLL"
        }
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

        filter {"system:linux", "configurations:*"}
            files { "src/system/pal_system_linux.c" }
        filter {}
    end

    if (PAL_BUILD_THREAD) then
        filter {"system:windows", "configurations:*"}
            files { "src/thread/pal_thread_win32.c" }

        filter {"system:linux", "configurations:*"}
            files { "src/thread/pal_thread_linux.c" }

        filter {}
    end

    if (PAL_BUILD_VIDEO) then
        filter {"system:windows", "configurations:*"}
            files { "src/video/pal_video_win32.c" }

        filter {"system:linux", "configurations:*"}
            files { "src/video/pal_video_linux.c" }

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
                defines { "PAL_HAS_WAYLAND=1" }
            else
                defines { "PAL_HAS_WAYLAND=0" }
            end

            -- check for X11 support. This is cross compiler
            local XPaths = {
                "/usr/include/X11/Xlib.h",
                "/usr/include/x86_64-linux-gnu/X11/Xlib.h"
            }

            found = false
            for _, path in ipairs(XPaths) do
                local file = io.open(path, "r")
                if file then
                    file:close()
                    found = true
                    break
                end
            end

            if found then
                defines { "PAL_HAS_X11=1" }
            else
                defines { "PAL_HAS_X11=0" }
            end

        filter {}
    end

    if (PAL_BUILD_OPENGL) then
        filter {"system:windows", "configurations:*"}
            files { "src/opengl/pal_opengl_win32.c" }

        filter {"system:linux", "configurations:*"}
            files { "src/opengl/pal_opengl_linux.c" }
        filter {}
    end

    if (PAL_BUILD_GRAPHICS) then
        -- check for vulkan support. This is cross compiler
        vulkan_sdk = os.getenv("VULKAN_SDK")
        hasVulkan = false
        if (vulkan_sdk) then
            hasVulkan = true
            -- add to include path if compiler does not see it
            includedirs {
                path.join(vulkan_sdk, "include")
            }

            libdirs {
                path.join(vulkan_sdk, "Lib")
            }

            defines { "PAL_HAS_VULKAN=1" }
        else
            defines { "PAL_HAS_VULKAN=0" }
        end

        -- base graphics file
        files { "src/graphics/pal_graphics.c" }

        filter {"system:windows", "configurations:*"}
            -- files { "src/graphics/pal_graphics_win32.c" }

        filter {"system:linux", "configurations:*"}
            if (hasVulkan) then
                files { "src/graphics/pal_vulkan.c" }
            end
        filter {}
    end

    writeConfig("include/pal/pal_config.h")
