
dofile("pal_config.lua")

project "PAL"
    if PAL_BUILD_STATIC_LIBRARY then
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
        -- core
        "src/core/pal_version.c",

        -- event
        "src/event/pal_default_queue.c",
        "src/event/pal_event.c"
    }

    filter {"system:windows", "configurations:*"}
        files {
            "src/core/win32/pal_log_win32.c",
            "src/core/win32/pal_memory_win32.c",
            "src/core/win32/pal_result_win32.c",
            "src/core/win32/pal_time_win32.c"
        }

    filter {"system:linux", "configurations:*"}
        files { 
            "src/core/linux/pal_log_linux.c",
            "src/core/linux/pal_memory_linux.c",
            "src/core/linux/pal_result_linux.c",
            "src/core/linux/pal_time_linux.c"
        }

    filter {}

    if (PAL_BUILD_SYSTEM_MODULE) then
        filter {"system:windows", "configurations:*"}
            files { "src/system/pal_system_win32.c" }

        filter {"system:linux", "configurations:*"}
            files { "src/system/pal_system_linux.c" }
        
        filter {}
    end

    if (PAL_BUILD_THREAD_MODULE) then
        filter {"system:windows", "configurations:*"}
            files { 
                "src/thread/win32/pal_condvar_win32.c",
                "src/thread/win32/pal_mutex_win32.c",
                "src/thread/win32/pal_tls_win32.c",
                "src/thread/win32/pal_thread_win32.c"
            }

        filter {"system:linux", "configurations:*"}
            files { 
                "src/thread/linux/pal_condvar_linux.c",
                "src/thread/linux/pal_mutex_linux.c",
                "src/thread/linux/pal_tls_linux.c",
                "src/thread/linux/pal_thread_linux.c"
            }

        filter {}
    end

    if (PAL_BUILD_VIDEO_MODULE) then
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
                defines { "PAL_HAS_WAYLAND_BACKEND=1" }
            else
                defines { "PAL_HAS_WAYLAND_BACKEND=0" }
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
                defines { "PAL_HAS_X11_BACKEND=1" }
            else
                defines { "PAL_HAS_X11_BACKEND=0" }
            end

        filter {}
    end

    if (PAL_BUILD_OPENGL_MODULE) then
        filter {"system:windows", "configurations:*"}
            files { "src/opengl/pal_opengl_win32.c" }

        filter {"system:linux", "configurations:*"}
            files { "src/opengl/pal_opengl_linux.c" }

        filter {}
    end

    if (PAL_BUILD_GRAPHICS_MODULE) then
        -- check for vulkan support. This is cross compiler
        local vulkanSdk = os.getenv("VULKAN_SDK")
        local hasVulkan = false
        if (vulkanSdk) then
            hasVulkan = true
            -- add to include path if compiler does not see it
            includedirs {
                path.join(vulkanSdk, "include")
            }

            defines { "PAL_HAS_VULKAN_BACKEND=1" }
        else
            defines { "PAL_HAS_VULKAN_BACKEND=0" }
        end

        -- check for d3d12 support. This is cross compiler
        local hasD3D12 = false
        local d3d12Include = ""
        if (_ACTION == "vs2022") or (_ACTION == "vs2026") then
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
            -- gccBasePath will be set if we are on gcc
            d3d12Include = path.join(gccBasePath, "include")
        end

        if (os.isfile(path.join(d3d12Include, "d3d12.h"))) then
            hasD3D12 = true
        end

        if (hasD3D12) then
            -- add to include path if compiler does not see it
            includedirs {
                d3d12Include
            }

            defines { "PAL_HAS_D3D12_BACKEND=1" }
        else
            defines { "PAL_HAS_D3D12_BACKEND=0" }
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
    end
