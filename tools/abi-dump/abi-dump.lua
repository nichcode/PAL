
project "abi-dump"
    kind "ConsoleApp"

    targetdir(targetDir)
    objdir(objDir)

    files { "src/**.c" }
    includedirs { "%{wks.location}/include", "src" }
    links { "PAL2" }
