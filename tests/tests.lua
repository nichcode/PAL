
local function addTest(file)
    local name = path.getbasename(file)
    project(name)
        kind "ConsoleApp"

        targetdir(targetDir)
        objdir(objDir)
        files("src/" .. file)

        -- TODO: remove comments includedirs { "%{wks.location}/include" }
        -- links { "PAL2" }
end

addTest("test_result.c")
addTest("test_allocator.c")