
project "tests"
    kind "ConsoleApp"

    targetdir(targetDir)
    objdir(objDir)

    files {
        "tests_main.c",
        "tests.c",

        -- core
        "core/logger_test.c",
        "core/time_test.c",

        -- event
        "event/user_event_test.c",
        "event/event_test.c"
    }

    if (PAL_BUILD_SYSTEM_MODULE) then
        files {
            "system/platform_test.c",
            "system/cpu_test.c"
        }
    end

    if (PAL_BUILD_THREAD_MODULE) then
        files {
            "thread/thread_test.c",
            "thread/tls_test.c",
            "thread/mutex_test.c",
            "thread/condvar_test.c"
        }
    end

    if (PAL_BUILD_VIDEO_MODULE) then
        files {
            "video/video_test.c",
            "video/monitor_test.c",
            "video/monitor_mode_test.c",
            "video/window_test.c",
            "video/icon_test.c",
            "video/cursor_test.c",
            "video/input_window_test.c",
            "video/system_cursor_test.c",
            "video/attach_window_test.c",
            "video/char_event_test.c",
            "video/native_integration_test.c",
            "video/native_instance_test.c",
            "video/custom_decoration_test.c"
        }
    end

    if (PAL_BUILD_OPENGL_MODULE and PAL_BUILD_VIDEO_MODULE) then
        files {
            "opengl/opengl_test.c",
            "opengl/opengl_fbconfig_test.c",
            "opengl/opengl_context_test.c",
            "opengl/opengl_multi_context_test.c",
            "opengl/opengl_helper.c"
        }
    end

    if (PAL_BUILD_OPENGL_MODULE and PAL_BUILD_VIDEO_MODULE and PAL_BUILD_THREAD_MODULE) then
        files {
            "opengl/multi_thread_opengl_test.c"
        }
    end

    if (PAL_BUILD_GRAPHICS_MODULE) then
        files {
            "graphics/graphics_test.c",
            "graphics/compute_test.c",
            "graphics/ray_tracing_test.c",
            "graphics/multi_descriptor_set_test.c",
            "graphics/custom_backend_test.c",
            "graphics/queue_ownership_test.c"
        }
    end

    if (PAL_BUILD_GRAPHICS_MODULE and PAL_BUILD_VIDEO_MODULE and PAL_BUILD_SYSTEM_MODULE) then
        files {
            "graphics/clear_color_test.c",
            "graphics/triangle_test.c",
            "graphics/mesh_test.c",
            "graphics/texture_test.c",
            "graphics/geometry_test.c",
            "graphics/indirect_draw_test.c",
            "graphics/descriptor_indexing_test.c",
            "graphics/graphics_helper.c"
        }
    end

    includedirs { 
        "%{wks.location}/include",
        "%{wks.location}/tests" 
    }
    
    links { "PAL2" }
