
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
            "window_update_bench.c"
        }
    end

    includedirs { "%{wks.location}/include" }
    links { "PAL" }