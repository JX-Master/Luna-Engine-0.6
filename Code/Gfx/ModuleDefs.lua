local module = {}
module.desc = "Gfx module provides uniform APIs to access underlying platform's graphic related functionalities."
module.dependencies = {"Base", "Core", "Input"}
module.on_config = function(self, builder)
    if builder.platform.static_link then
        builder:kind("lib")
    else
        builder:kind("dll")
    end
    if builder.platform.platform == "windows" then
        if builder.platform.render_api == "d3d11" then
            builder:defines({"LUNA_GFX_D3D11"})
        else
            builder:defines({"LUNA_GFX_D3D12"})
        end
    else
        error("Gfx Module is not implemented on " .. builder.platform.platform)
    end
    builder:files({"**.h", "**.hpp", "**.c", "**.cpp", "**.inl"})
end
    
return module 