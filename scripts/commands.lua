local function escape(value)
    return value:gsub('\\', '\\\\'):gsub('"', '\\"')
end

function generateCompileCommands()
    print("\n=======================================================")
    print("Generating build/compile_commands.json")

    os.mkdir("build")
    local file = io.open("build/compile_commands.json", "w")
    if not file then
        print("Failed to write to build/compile_commands.json")
        return
    end

    local entries = {}
    local workspace = premake.global.getWorkspace(workspaceName)

    for prj in premake.workspace.eachproject(workspace) do
        local flags = {}

        if _ACTION == "gmake" then
            if _OPTIONS["compiler"] == "clang" then
                table.insert(flags, os.target() == "windows" and "clang.exe" or "clang")
                for _, mf in ipairs(mingwFlags or {}) do
                    table.insert(flags, " " .. mf)
                end
            else
                table.insert(flags, os.target() == "windows" and "gcc.exe" or "gcc")
            end

            table.insert(flags, " -g -m64 -std=c99 -Wno-switch -Wno-switch-enum")
            table.insert(flags, " -Wall -Wextra -Wpedantic -Wconversion")
            table.insert(flags, " -Wsign-conversion -Werror")

            if prj.kind == "SharedLib" then
                table.insert(flags, " -fPIC")
            else
                table.insert(flags, " -O3")
            end

            for _, dir in ipairs(prj.includedirs or {}) do
                table.insert(flags, string.format(' -I "%s"', dir))
            end

            for _, define in ipairs(prj.defines or {}) do
                table.insert(flags, " -D " .. define)
            end

        elseif _ACTION == "vs2022" or _ACTION == "vs2026" then
            table.insert(flags, "clang-cl.exe")
            table.insert(flags, " /Zi /std:c11 /W4 /WX /wd6387")
            table.insert(flags, " /wd4018 /wd4133 /wd4101")

            for _, dir in ipairs(prj.includedirs or {}) do
                table.insert(flags, string.format(' /I "%s"', dir))
            end

            for _, define in ipairs(prj.defines or {}) do
                table.insert(flags, " /D " .. define)
            end
        end

        local cmdBase = table.concat(flags, "")
        for _, f in ipairs(prj.files) do
            local command = string.format('%s %s', cmdBase, f)
            local entry = table.concat({
                '    {',
                string.format('        "directory": "%s",', escape(workspace.location)),
                string.format('        "file": "%s",', escape(f)),
                string.format('        "command": "%s"', escape(command)),
                '    }'
            }, "\n")
            
            table.insert(entries, entry)
        end
    end

    file:write('[\n')
    file:write(table.concat(entries, ",\n\n"))

    if #entries > 0 then
        file:write('\n')
    end

    file:write(']\n')
    file:close()
end