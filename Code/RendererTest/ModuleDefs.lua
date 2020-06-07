local module = {}
module.desc = ""
module.dependencies = {"Base", "Core", "Input", "Gfx", "Renderer"}
module.on_config = function(self, builder)
    builder:kind("exe-console")
    builder:files({"**.h", "**.hpp", "**.c", "**.cpp", "**.inl"})
end
    
return module 