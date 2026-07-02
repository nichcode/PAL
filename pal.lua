
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
            "src/core/posix/pal_log_posix.c",
            "src/core/posix/pal_memory_posix.c",
            "src/core/posix/pal_result_posix.c",
            "src/core/posix/pal_time_posix.c"
        }

    filter {}

    if (PAL_BUILD_SYSTEM_MODULE) then
        filter {"system:windows", "configurations:*"}
            files { 
                "src/system/win32/pal_cpu_win32.c",
                "src/system/win32/pal_platform_win32.c" 
            }

        filter {"system:linux", "configurations:*"}
            files { 
                "src/system/linux/pal_cpu_linux.c",
                "src/system/linux/pal_platform_linux.c" 
            }
        
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
                "src/thread/posix/pal_condvar_posix.c",
                "src/thread/posix/pal_mutex_posix.c",
                "src/thread/posix/pal_tls_posix.c",
                "src/thread/posix/pal_thread_posix.c"
            }

        filter {}
    end

    if (PAL_BUILD_VIDEO_MODULE) then
        files { "src/video/pal_video.c" }

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
        end

        filter {"system:windows", "configurations:*"}
            files { 
                "src/video/win32/pal_cursor_win32.c",
                "src/video/win32/pal_icon_win32.c",
                "src/video/win32/pal_monitor_win32.c",
                "src/video/win32/pal_window_win32.c",
                "src/video/win32/pal_video_win32.c"
            }

        filter {"system:linux", "configurations:*"}
            files {
                -- X11
                "src/video/x11/pal_cursor_x11.c",
                "src/video/x11/pal_icon_x11.c",
                "src/video/x11/pal_monitor_x11.c",
                "src/video/x11/pal_window_x11.c",
                "src/video/x11/pal_video_x11.c",

                -- Wayland
                "src/video/wayland/pal_cursor_wayland.c",
                "src/video/wayland/pal_icon_wayland.c",
                "src/video/wayland/pal_monitor_wayland.c",
                "src/video/wayland/pal_window_wayland.c",
                "src/video/wayland/pal_wayland_protocols.c",
                "src/video/wayland/pal_video_wayland.c"
            }
            
        filter {}
    end

    if (PAL_BUILD_OPENGL_MODULE) then
        files { "src/opengl/pal_opengl.c" }

        filter {"system:windows", "configurations:*"}
            files { 
                "src/opengl/wgl/pal_context_wgl.c",
                "src/opengl/wgl/pal_wgl.c"
            }

        filter {"system:linux", "configurations:*"}
            files { 
                "src/opengl/egl/pal_context_egl.c",
                "src/opengl/egl/pal_egl.c"
            }

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
        if (hasVulkan) then
            files { 
                "src/graphics/vulkan/pal_adapter_vulkan.c",
                "src/graphics/vulkan/pal_as_vulkan.c",
                "src/graphics/vulkan/pal_buffer_vulkan.c",
                "src/graphics/vulkan/pal_command_pool_vulkan.c",
                "src/graphics/vulkan/pal_commands_vulkan.c",
                "src/graphics/vulkan/pal_descriptors_vulkan.c",
                "src/graphics/vulkan/pal_device_vulkan.c",
                "src/graphics/vulkan/pal_image_vulkan.c",
                "src/graphics/vulkan/pal_pipeline_vulkan.c",
                "src/graphics/vulkan/pal_sbt_vulkan.c",
                "src/graphics/vulkan/pal_swapchain_vulkan.c",
                "src/graphics/vulkan/pal_sync_vulkan.c",
                "src/graphics/vulkan/pal_vulkan.c"
            }
        end

        if (hasD3D12) then
            files { 
                -- "src/graphics/pal_d3d12.c" 
            }
        end
    end
