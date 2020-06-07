-- luna file system

local fs = {}
local lfs = require "lfs"
local lstr = require "lunastr"

-- copy a file from `from_path` to `to_path`
fs.copy_file = function(from_path, to_path)
    -- print("copy file " .. from_path .. " to " .. to_path)
    local from_file = assert(io.open(from_path, "rb"))
    local to_file = assert(io.open(to_path, "wb"))
    local t = from_file:read("a")
    to_file:write(t)
    from_file:close()
    to_file:close()
end

-- check if the path given is ended with `/`, and append `/` to the end if not. 
fs.format_as_dir = function(dir)
    dir = string.gsub(dir, "\\", "/")
    dir = string.gsub(dir, ":", "/")
    if not string.find(dir, "/", #dir) then
        return dir .. "/"
    else
        return dir
    end
end

fs.get_extension = function(dir)
    local rev = string.reverse(dir)
    local place = string.find(rev, "%.")
    if not place then
        return nil
    end
    local substr = string.sub(rev, 1, place - 1)
    return string.reverse(substr)
end

fs.remove_extension = function(dir)
    local rev = string.reverse(dir)
    local place = string.find(rev, "%.")
    if not place then
        return nil
    end
    local substr = string.sub(rev, place + 1, -1)
    return string.reverse(substr)
end

-- copy all contexts in `from_dir` to `to_dir`.
fs.copy_dir_content = function(from_dir, to_dir)
    from_dir = fs.format_as_dir(from_dir)
    to_dir = fs.format_as_dir(to_dir)
    -- print("copy content in" .. from_dir .. " to " .. to_dir)
    for file in lfs.dir(from_dir) do
        -- print("file " .. file)
        if file ~= "." and file ~= ".." then
            local attr = lfs.attributes(from_dir .. file)
            if attr.mode == "directory" then
                -- as dir.
                lfs.mkdir(to_dir .. file)
                fs.copy_dir_content(from_dir .. file, to_dir .. file)
            else
                -- as file.
                fs.copy_file(from_dir .. file, to_dir .. file)
            end
        end
    end
end

-- copy a directory from `from_path` to `to_path`
fs.copy_folder = function(from_dir, to_dir)
    lfs.mkdir(to_dir)
    fs.copy_dir_content(from_dir, to_dir)
end

-- create all directories to the target directory.
fs.create_path = function(dir_path)
    dir_path = fs.format_as_dir(dir_path)
    local dirs = lstr.split(dir_path, "/")
    local s = ""
    for i = 1, #dirs do
        s = s .. dirs[i] .. "/"
        if lfs.attributes(s) == nil then
            lfs.mkdir(s)
        end
    end
end

-- generate relative path from src to dest.
fs.create_relative_path = function(dest, src)
    dest = fs.format_as_dir(dest)
    src = fs.format_as_dir(src)
    local dests = lstr.split(dest, "/")
    local srcs = lstr.split(src, "/")
    local i = 1
    while dests[i] == srcs[i] do i = i + 1 end
    local s = ""
    for j = i, #srcs do
        s = s .. "../"
    end
    for j = i, #dests do
        s = s .. dests[j] .. "/"
    end
    return s
end

return fs