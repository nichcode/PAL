
project "collision_test"
    language "C++"
    kind "ConsoleApp"

    targetdir(target_dir)
    objdir(obj_dir) 

    files { "collision_test.cpp" }
    includedirs { "%{wks.location}/include" }
    links { "PAL" }