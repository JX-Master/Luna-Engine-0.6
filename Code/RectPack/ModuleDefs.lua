local module = {}
module.desc = "Provides functionalities to pack multiple rects into one single rect, mainly used for texture atlas."
module.dependencies = {"Base", "Core"}
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