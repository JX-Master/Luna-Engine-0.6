local module = {}
module.desc = "ImGuizmo"
module.sdks = {"ImGuiLib"}
module.on_config = function(self, builder)
    builder:kind("lib")
    builder:includedirs({"/SDKs/ImGuiLib/"})
    builder:files({"ImGuizmo.h", "ImGuizmo.cpp"})
end
return module