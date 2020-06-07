local module = {}
module.on_config = function(self, builder)
    builder:kind("lib")
    builder:files({"**.h", "**.hpp", "**.c", "**.cpp", "**.inl"})
end
    
return module