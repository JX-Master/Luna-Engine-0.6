local module = {}
module.desc = ""
module.dependencies = {"Runtime", "Core", "Input", "Gfx"}
module.on_config = function(self, builder)
    builder:kind("exe-console")
    builder:files({"**.h", "**.hpp", "**.c", "**.cpp", "**.inl"})
end
    
return module 