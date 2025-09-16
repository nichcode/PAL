
project "bench"
    language "C"
    kind "ConsoleApp"

    targetdir(target_dir)
    objdir(obj_dir)

    files { 
        "bench_main.c",
        "bench.c"
    }

    includedirs { "%{wks.location}/include" }
    links { "PAL" }