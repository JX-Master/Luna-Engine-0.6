local module = {}
module.desc = "Provides functionalities to load and rasterize fonts."
module.dependencies = {"Runtime", "Core", "Gfx", "Image", "RectPack"}
module.sdks = {"Stb"}
module.on_config = function(self, builder)
    if builder.platform.static_link then
        builder:kind("lib")
    else
        builder:kind("dll")
    end
    builder:files({"**.h", "**.hpp", "**.c", "**.cpp", "**.inl"})
end
    
return module