local registeredTests = {}

local function addTest(file)
    local name = path.getbasename(file)
    table.insert(registeredTests, name)

    project(name)
        kind "ConsoleApp"

        targetdir(targetDir)
        objdir(objDir)
        files(file)

        includedirs { "%{wks.location}/include" }
        links { "PAL2" }
end

function generateTests()
    print("\n=======================================================")
    print("Generating build/tests.json")

    os.mkdir("build")
    local file = io.open("build/tests.json", "w")
    if not file then
        print("Failed to write to build/tests.json")
        return
    end

    local entries = {}
    for _, name in ipairs(registeredTests) do
        if name:sub(1, 5) == "test_" then
            table.insert(entries, string.format('    "%s"', name))
        end
    end

    file:write('[\n')
    file:write(table.concat(entries, ",\n"))

    if #entries > 0 then
        file:write('\n')
    end
    
    file:write(']\n')
    file:close()
end

local coreFiles = os.matchfiles("src/core/*.c")
for _, file in ipairs(coreFiles) do
    addTest(file)
end

if PAL_BUILD_EVENT_MODULE then
    local eventFiles = os.matchfiles("src/event/*.c")
    for _, file in ipairs(eventFiles) do
        addTest(file)
    end
end

if PAL_BUILD_SYSTEM_MODULE then
    local systemFiles = os.matchfiles("src/system/*.c")
    for _, file in ipairs(systemFiles) do
        addTest(file)
    end
end

if PAL_BUILD_THREAD_MODULE then
    local threadFiles = os.matchfiles("src/thread/*.c")
    for _, file in ipairs(threadFiles) do
        addTest(file)
    end
end

if PAL_BUILD_VIDEO_MODULE then
    local videoFiles = os.matchfiles("src/video/*.c")
    for _, file in ipairs(videoFiles) do
        addTest(file)
    end
end

if PAL_BUILD_OPENGL_MODULE and PAL_BUILD_VIDEO_MODULE and PAL_BUILD_EVENT_MODULE then
    local openglFiles = os.matchfiles("src/opengl/*.c")
    for _, file in ipairs(openglFiles) do
        addTest(file)
    end
end