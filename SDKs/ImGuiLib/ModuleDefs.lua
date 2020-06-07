local module = {}
module.desc = "Dear ImGui v1.75"
module.on_config = function(self, builder)
    builder:kind("lib")
    builder:files({"**.h", "**.cpp"})
end
return module