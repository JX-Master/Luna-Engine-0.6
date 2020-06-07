local module = {}
module.desc = "stb library - https://github.com/nothings/stb"
module.on_config = function(self, builder)
    builder:kind("utility")
    builder:files({"**.h", "**.hpp", "**.cpp"})
end
return module