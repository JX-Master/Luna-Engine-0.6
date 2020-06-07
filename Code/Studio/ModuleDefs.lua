local module = {}
module.desc = ""
module.dependencies = {"Base", "Core", "Input", "Gfx", "Renderer", "RectPack", 
"Image", "Font", "ImGui", "Asset", "Texture", "3DEngine", "ObjLoader", "Scene"}
module.on_config = function(self, builder)
    builder:kind("exe-console")
    builder:files({"**.h", "**.hpp", "**.c", "**.cpp", "**.inl", "**.hlsl"})
end
    
return module 