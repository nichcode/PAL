
project "Tests"
    language "C"
    kind "ConsoleApp"

    targetdir(target_dir)
    objdir(obj_dir) 

    -- core
    files { 
        "src/log_test.c", 
        "src/timer_test.c",

        "src/main.c"
    }

    if PAL_BUILD_VIDEO then
        files {
            "src/display_mode_test.c",
            "src/display_test.c",
            "src/video_test.c",
            "src/window_test.c"
        }
    end

    if PAL_BUILD_EVENT then
        files {
            "src/event_test.c"
        }
    end

    if PAL_BUILD_INPUT then
        files {
            "src/input_test.c"
        }
    end

    includedirs { "%{wks.location}/include" }
    links { "PAL" }