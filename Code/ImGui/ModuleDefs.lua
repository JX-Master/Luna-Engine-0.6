local module = {}
module.desc = "Provides functionalities to draw simple 2D geometry to any surface."
module.dependencies = {"Base", "Core", "Gfx", "Input", "Font"}
module.sdks = {"ImGuiLib", "ImGuizmo"}
module.on_config = function(self, builder)
    if builder.platform.static_link then
        builder:kind("lib")
    else
        builder:kind("dll")
    end
    builder:files({"**.h", "**.hpp", "**.c", "**.cpp", "**.inl"})
end
    
return module