
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

local coreFiles = os.matchfiles("src/core/*.c")
for _, file in ipairs(coreFiles) do
    addTest(file)
end


if (PAL_BUILD_EVENT_MODULE) then
    local eventFiles = os.matchfiles("src/event/*.c")
    for _, file in ipairs(eventFiles) do
        addTest(file)
    end
end


if (PAL_BUILD_SYSTEM_MODULE) then
    local systemFiles = os.matchfiles("src/system/*.c")
    for _, file in ipairs(systemFiles) do
        addTest(file)
    end
end


if (PAL_BUILD_THREAD_MODULE) then
    local threadFiles = os.matchfiles("src/thread/*.c")
    for _, file in ipairs(threadFiles) do
        addTest(file)
    end
end


if (PAL_BUILD_VIDEO_MODULE) then
    local videoFiles = os.matchfiles("src/video/*.c")
    for _, file in ipairs(videoFiles) do
        addTest(file)
    end
end


if (PAL_BUILD_OPENGL_MODULE) then
    local openglFiles = os.matchfiles("src/opengl/*.c")
    for _, file in ipairs(openglFiles) do
        addTest(file)
    end
end