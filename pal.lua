
dofile("pal_config.lua")

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

    targetdir(targetDir)
    objdir(objDir)

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
        defines { "PAL_HAS_SYSTEM_MODULE = 1" }
    else
        defines { "PAL_HAS_SYSTEM_MODULE = 0" }
    end

    if (PAL_BUILD_THREAD) then
        filter {"system:windows", "configurations:*"}
            files { "src/thread/pal_thread_win32.c" }

        filter {"system:linux", "configurations:*"}
            files { "src/thread/pal_thread_linux.c" }

        filter {}
        defines { "PAL_HAS_THREAD_MODULE = 1" }
    else
        defines { "PAL_HAS_THREAD_MODULE = 0" }
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
                defines { "PAL_HAS_WAYLAND_BACKEND = 1" }
            else
                defines { "PAL_HAS_WAYLAND_BACKEND = 0" }
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
                defines { "PAL_HAS_X11_BACKEND = 1" }
            else
                defines { "PAL_HAS_X11_BACKEND = 0" }
            end

        filter {}
        defines { "PAL_HAS_VIDEO_MODULE = 1" }
    else
        defines { "PAL_HAS_VIDEO_MODULE = 0" }
    end

    if (PAL_BUILD_OPENGL) then
        filter {"system:windows", "configurations:*"}
            files { "src/opengl/pal_opengl_win32.c" }

        filter {"system:linux", "configurations:*"}
            files { "src/opengl/pal_opengl_linux.c" }

        filter {}
        defines { "PAL_HAS_OPENGL_MODULE = 1" }
    else
        defines { "PAL_HAS_OPENGL_MODULE = 0" }
    end

    if (PAL_BUILD_GRAPHICS) then
        -- check for vulkan support. This is cross compiler
        local vulkanSdk = os.getenv("VULKAN_SDK")
        local hasVulkan = false
        if (vulkanSdk) then
            hasVulkan = true
            -- add to include path if compiler does not see it
            includedirs {
                path.join(vulkanSdk, "include")
            }

            defines { "PAL_HAS_VULKAN_BACKEND = 1" }
        else
            defines { "PAL_HAS_VULKAN_BACKEND = 0" }
        end

        -- check for d3d12 support. This is cross compiler
        local hasD3D12 = false
        local d3d12Include = os.getenv("D3D12_INCLUDE")
        if (os.isfile(path.join(d3d12Include, "d3d12.h"))) then
            hasD3D12 = true

        else
            if (_ACTION == "vs2022") or (_ACTION == "vs2026") then
                d3d12Include = ""
                local base = "C:/Program Files (x86)/Windows Kits/10/Include"
                local versions = os.matchdirs(base .. "/*")
                table.sort(versions)

                for i = #versions, 1, -1 do
                    local v = versions[i]
                    d3d12Include = path.join(v, "um")
                    if (os.isdir(d3d12Include)) then
                        break
                    end
                end
            else
                d3d12Include = path.join(ucrt, "include")
            end

            if (os.isfile(path.join(d3d12Include, "d3d12.h"))) then
                hasD3D12 = true
            end
        end

        if (hasD3D12) then
            -- add to include path if compiler does not see it
            includedirs {
                d3d12Include
            }

            defines { "PAL_HAS_D3D12_BACKEND = 1" }
        else
            defines { "PAL_HAS_D3D12_BACKEND = 0" }
        end

        -- base graphics file
        files { "src/graphics/pal_graphics.c" }

        filter {"system:windows", "configurations:*"}
            if (hasVulkan) then
                files { "src/graphics/pal_vulkan.c" }
            end

            if (hasD3D12) then
                files { "src/graphics/pal_d3d12.c" }
            end

        filter {"system:linux", "configurations:*"}
            if (hasVulkan) then
                files { "src/graphics/pal_vulkan.c" }
            end
            
        filter {}
        defines { "PAL_HAS_GRAPHICS_MODULE = 1" }
    else
        defines { "PAL_HAS_GRAPHICS_MODULE = 0" }
    end
