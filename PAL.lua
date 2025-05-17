
project "PAL"
    language "C++"

if (_OPTIONS["with-shared"]) then

    kind "SharedLib"
    staticruntime "off"
    defines { "PAL_DLL" }

else
    kind "StaticLib"
    staticruntime "on"
end

    targetdir(target_dir)
    objdir(obj_dir)

    pchheader "src/PAL_pch.h"
    files { "src/PAL_build.cpp" }

    includedirs {
        "include",
        "src"
    }