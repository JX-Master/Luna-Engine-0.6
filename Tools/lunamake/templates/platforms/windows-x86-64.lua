
local t = {}
t.produce = function(...)
    local p = {}
    p.platform = "windows"
    p.architecture = "x86-64"
    p.compiler = "msvc"
    p.static_link = false
    return p
end
return t