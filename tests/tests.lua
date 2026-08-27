
project "tests"
    kind "ConsoleApp"

    targetdir(targetDir)
    objdir(objDir)

    files {
        "tests_main.c",
        "tests.c",
        "core_tests.c",
        "event_tests.c"
    }
    
    includedirs { 
        "%{wks.location}/include",
        "%{wks.location}/tests" 
    }
    
    links { "PAL2" }
