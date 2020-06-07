
local t = {}
t.produce = function(...)
    local p = {}
    p.platform = "windows"
    p.architecture = "x86"
    p.compiler = "msvc"
    p.static_link = true
    return p
end
return t