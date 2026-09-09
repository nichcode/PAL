
-- All tests have the same structure so we create a function
-- that checks all tests file and check projects for each.
-- The limitation is all tests must be exactly 1 file.
local function addTest(file)
    local name = path.getbasename(file)
    project(name)
        kind "ConsoleApp"

        targetdir(targetDir)
        objdir(objDir)
        files(file)

        includedirs { "%{wks.location}/include" }
        links { "PAL2" }
end

-- Core tests
local coreFiles = os.matchfiles("src/core/*.c")
for _, file in ipairs(coreFiles) do
    addTest(file)
end