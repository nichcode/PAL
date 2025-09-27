
project "bench"
    language "C"
    kind "ConsoleApp"

    targetdir(target_dir)
    objdir(obj_dir)

    files { 
        "bench_main.c",
        "bench.c"
    }

    if (PAL_BUILD_VIDEO) then
        files { 
            "window_bench.c",
            "window_bench_platform.c"
        }
    end

    includedirs { "%{wks.location}/include" }
    links { "PAL" }