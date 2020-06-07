local module = {}
module.desc = "Tiny Object Loader"
module.on_config = function(self, builder)
    builder:kind("lib")
    builder:files({"tiny_obj_loader.h", "tiny_obj_loader.cc"})
end
return module