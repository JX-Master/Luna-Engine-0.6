local module = {}
module.desc = "lua 5.3 sdk"
module.on_config = function(self, builder)
    builder:kind("lib")
    builder:files({"**.h", "**.hpp", "**.c"})
    builder:removefiles({"lua.c", "luac.c"})
end
return module