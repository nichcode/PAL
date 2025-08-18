
project "Tests"
    language "C"
    kind "ConsoleApp"

    targetdir(target_dir)
    objdir(obj_dir)

    files { 
        "src/main.c",
        "src/allocator_test.c",
        "src/time_test.c",
        "src/events_test.c"
    }

    if (PAL_BUILD_VIDEO) then
        files { 
            "src/video_test.c",
            "src/display_test.c",
            "src/display_mode_test.c",
            "src/window_test.c",
            "src/multi_window_test.c"
        }
    end

    if (PAL_BUILD_INPUT) then
        files { 
            "src/input_test.c",
            "src/keyboard_test.c",
            "src/mouse_test.c",
            "src/keyboard_mouse_test.c",
            "src/gamepad_test.c"
        }
    end

    if (PAL_BUILD_OPENGL) then
        files { 
            "src/opengl_test.c",
        }
    end

    if (PAL_BUILD_OPENGL and PAL_BUILD_VIDEO) then
        files { 
            "src/opengl_pixel_test.c"
        }
    end

    includedirs { "%{wks.location}/include" }
    links { "PAL" }