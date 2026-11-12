
project "abi-dump"
    kind "ConsoleApp"

    targetdir(targetDir)
    objdir(objDir)

    files { "src/**.c" }
    includedirs { 
        "%{wks.location}/include", 
        "%{wks.location}/tools/abi-dump/src"
    }
    links { "PAL2" }
