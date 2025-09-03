
project "tests"
    language "C"
    kind "ConsoleApp"

    targetdir(target_dir)
    objdir(obj_dir)

    files { 
        "tests_main.c",
        "tests.c",
        "logger_test.c",
        "time_test.c"
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
            "display_test.c"
        }
    end

    includedirs { "%{wks.location}/include" }
    links { "PAL" }