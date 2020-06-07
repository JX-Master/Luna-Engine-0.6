local module = {}
module.desc = ""
module.dependencies = {"Base", "Core", "Asset", "Gfx", "Renderer", "Image"}
module.on_config = function(self, builder)
    if builder.platform.static_link then
        builder:kind("lib")
    else
        builder:kind("dll")
    end
    builder:files({"**.h", "**.hpp", "**.c", "**.cpp", "**.inl"})
end
    
return module 