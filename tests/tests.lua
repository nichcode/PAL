
project "tests"
    language "C"
    kind "ConsoleApp"

    targetdir(target_dir)
    objdir(obj_dir)

    files { 
        "tests_main.c",
        "tests.c",
        "logger_test.c",
        "time_test.c",
        "user_event_test.c",
    }

    if (PAL_BUILD_SYSTEM) then
        files { 
            "system_test.c"
        }
    end

    if (PAL_BUILD_THREAD) then
        files { 
            "thread_test.c",
            "tls_test.c",
            "mutex_test.c",
            "condvar_test.c"
        }
    end

    if (PAL_BUILD_VIDEO) then
        files { 
            "video_test.c",
            "monitor_test.c",
            "monitor_mode_test.c",
            "window_test.c",
            "icon_test.c",
            "cursor_test.c",
            "input_window_test.c"
        }
    end

    if (PAL_BUILD_OPENGL) then
        files { 
            "opengl_test.c",
            opengl_fbconfig_test
        }
    end

    if (PAL_BUILD_OPENGL and PAL_BUILD_VIDEO) then
        files { 
            "opengl_fbconfig_test.c",
            "opengl_context_test.c"
        }
    end

    includedirs { "%{wks.location}/include" }
    links { "PAL" }