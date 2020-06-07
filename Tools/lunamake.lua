-- lunamake project source code.

local lfs = require "lfs"
local lunafs = require "lunafs"
local lstr = require "lunastr"
local lunamake = {}

lunamake._load_project = function(project_path)
    project_path = lunafs.format_as_dir(project_path)
    local proj = assert(dofile(project_path .. "ProjectDefs.lua"))
    -- fill proj object.
    if not proj.name then
        error("Project Name Underifned for file " .. project_path .. "ProjectDefs.lua")
    end
    if not proj.desc then
        proj.desc = ""
    end
    if not proj.code_path then
        proj.code_path = "./Code/"
    end
    if not proj.sdk_path then
        proj.sdk_path = "./SDKs/"
    end
    -- load modules if needed.
    if not proj.modules or next(proj.modules) == nil then
        -- fill out module list if not specified.
        proj.modules = {}
        for file in lfs.dir(project_path .. proj.code_path) do
            if file ~= "." and file ~= ".." then
                local file_path = project_path .. proj.code_path .. file
                local attr = lfs.attributes(file_path)
                if attr.mode == "directory" then
                    local def_attr = lfs.attributes(file_path .. "/ModuleDefs.lua")
                    if def_attr then
                        proj.modules[#proj.modules + 1] = file
                    end
                end
            end
        end
    end
    -- load sdks if needed.
    if not proj.sdks or next(proj.sdks) == nil then
        -- fill out module list if not specified.
        proj.sdks = {}
        for file in lfs.dir(project_path .. proj.sdk_path) do
            if file ~= "." and file ~= ".." then
                local file_path = project_path .. proj.sdk_path .. file
                local attr = lfs.attributes(file_path)
                if attr.mode == "directory" then
                    local def_attr = lfs.attributes(file_path .. "/ModuleDefs.lua")
                    if def_attr then
                        proj.sdks[#proj.sdks + 1] = file
                    end
                end
            end
        end
    end
    proj._module_reg = {}
    for _k, v in pairs(proj.modules) do
        -- used to looking for modules.
        proj._module_reg[v] = true
    end
    proj._sdk_reg = {}
    for _k, v in pairs(proj.sdks) do
        -- used to looking for sdks.
        proj._sdk_reg[v] = true
    end
    -- load parents.
    if proj.parent_path and #proj.parent_path ~= 0 then
        proj.parent = lunamake._load_project(project_path .. proj.parent_path)
    end
    proj.path = project_path
    print("[lunamake] Load project " .. proj.name .. " from " .. project_path)
    return proj
end

-- Create a new solution context and reads the project specified into the context.
-- One context can only load one project, and the project cannot be changed
-- after the context is created.
lunamake.new_solution = function(solution_name, project_path)
    local sln = {}
    sln.name = solution_name
    sln.output_path = "./Solution/"
    sln.project = lunamake._load_project(project_path)
    sln.modules = {}
    sln.sdks = {}
    sln.cases = {}
    sln.platforms = {}

    sln.add_case = function(self, case_name, case)
        self.cases[case_name] = case
    end

    sln.add_platform = function(self, platform_name, platform)
        self.platforms[platform_name] = platform
    end

    sln.add_module = function(self, module_name)
        -- module will not be reloaded.
        if self.modules[module_name] ~= nil then
            return
        end

        -- find project that contains the module.
        local proj = self.project
        while proj and (not proj._module_reg[module_name]) do
            proj = proj.parent
        end

        if not proj then
            error("Module " .. module_name .. " cannot be resolved.")
        end

        local mod = assert(dofile(proj.path .. proj.code_path .. module_name .. "/ModuleDefs.lua"))
        if not mod.desc then
            mod.desc = ""
        end
        if not mod.params then
            mod.params = {}
        end
        if type(mod.on_config) ~= "function" then
            error("Module on_config callback not found for module " .. module_name)
        end
        mod.belonging_project = proj
        self.modules[module_name] = mod
        print("[lunamake] Load module " .. module_name .. " from project " .. mod.belonging_project.name)

        mod.dependencies = mod.dependencies or {}
        -- resolve dependencies.
        for _k, v in pairs(mod.dependencies) do
            if not pcall(self.add_module, self, v) then
                error("Module " .. v .. " required by module " .. module_name .. " cannot be found.")
            end
        end

        -- resolve sdks.
        if mod.sdks then
            for _k, v in pairs(mod.sdks) do
                if not pcall(self._add_sdk, self, v) then
                    error("SDK module " .. v .. " required by module " .. module_name .. " cannot be found.")
                end
            end
        end
    end

    sln.add_all_modules = function(self)
        for _k, v in pairs(self.project.modules) do
            self:add_module(v)
        end
    end

    sln._add_sdk = function(self, sdk_name)
         -- sdk will not be reloaded.
         if self.sdks[sdk_name] ~= nil then
            return
        end

        -- find project that contains the module.
        local proj = self.project
        while proj and (not proj._sdk_reg[sdk_name]) do
            proj = proj.parent
        end

        if not proj then
            error("SDK Module " .. sdk_name .. " cannot be resolved.")
        end

        local mod = assert(dofile(proj.path .. proj.sdk_path .. sdk_name .. "/ModuleDefs.lua"))
        if not mod.desc then
            mod.desc = ""
        end
        if not mod.params then
            mod.params = {}
        end
        if type(mod.on_config) ~= "function" then
            error("Module on_config callback not found for SDK module " .. sdk_name)
        end
        mod.belonging_project = proj
        mod.is_sdk = true
        self.sdks[sdk_name] = mod

        -- resolve dependencies.
        if mod.dependencies then
            for _k, v in pairs(mod.dependencies) do
                if not pcall(self.add_module, self, v) then
                    error("Module " .. v .. " required by SDK module " .. module_name .. " cannot be found.")
                end
            end
        end

        -- resolve sdks.
        if mod.sdks then
            for _k, v in pairs(mod.sdks) do
                if not pcall(self._add_sdk, self, v) then
                    error("SDK module " .. v .. " required by SDK module " .. module_name .. " cannot be found.")
                end
            end
        end
    end

    sln._generate_module = function(self, b, module_name, module)
        b:_entry("project", module_name)
        b.module_name = module_name
        b.module = module
        for case_name, case in pairs(self.cases) do
            b.case_name = case_name
            b.case = case
            for platform_name, platform in pairs(self.platforms) do
                b:_append("filter { \"configurations:" .. case_name .. "\", \"platforms:" .. platform_name .. "\"}\n")
                b.platform_name = platform_name
                b.platform = platform
                -- Dump global items.
                b:defines({"LUNA_MANUAL_CONFIG_CPP_STANDARD_VERSION", "LUNA_COMPILER_CPP14"})
                if b.case.debug_level then
                    b:defines({"LUNA_MANUAL_CONFIG_DEBUG_LEVEL"})
                    if b.case.debug_level == "debug" then
                        b:defines({"LUNA_DEBUG_LEVEL=2"})
                        b:symbols("on")
                        b:optimize("off")
                    elseif b.case.debug_level == "profile" then
                        b:defines({"LUNA_DEBUG_LEVEL=1"})
                        b:symbols("off")
                        b:optimize("speed")
                    elseif b.case.debug_level == "release" then
                        b:defines({"LUNA_DEBUG_LEVEL=0"})
                        b:symbols("off")
                        b:optimize("full")
                    end
                end
                if b.platform.platform then
                    b:defines({"LUNA_MANUAL_CONFIG_PLATFORM"})
                    if b.platform.platform == "windows" then
                        b:system("windows")
                        b:defines({"LUNA_PLATFORM_WINDOWS"})
                    elseif b.platform.platform == "linux" then
                        b:system("linux")
                        b:defines({"LUNA_PLATFORM_LINUX"})
                    else
                        error ("Unsupported platform.platform specified: " .. b.platform.platform)
                    end
                end
                if b.platform.architecture then
                    b:defines({"LUNA_MANUAL_CONFIG_CPU_ARCHITECTURE"})
                    if b.platform.architecture == "x86" then
                        b:architecture("x86")
                        b:defines({"LUNA_PLATFORM_X86"})
                    elseif b.platform.architecture == "x86-64" then
                        b:architecture("x86-64")
                        b:defines({"LUNA_PLATFORM_X86_64"})
                    elseif b.platform.architecture == "arm" then
                        b:architecture("arm")
                        b:defines({"LUNA_PLATFORM_ARM"})
                    else
                        error("Unsupported CPU architecture specified: " .. b.platform.architecture)
                    end
                end
                if b.platform.compiler then
                    b:defines({"LUNA_MANUAL_CONFIG_COMPILER"})
                    if b.platform.compiler == "msvc" then
                        b:compiler("msvc")
                        b:defines({"LUNA_COMPILER_MSVC"})
                    elseif b.platform.compiler == "gcc" then
                        b:compiler("gcc")
                        b:defines({"LUNA_COMPILER_GCC"})
                    elseif b.platform.compiler == "clang" then
                        b:compiler("clang")
                        b:defines({"LUNA_COMPILER_CLANG"})
                    else
                        error("Unsupported compiler toolchain specified: " .. b.platform.compiler)
                    end
                end

                -- global include paths.
                b:includedirs({
                    self.project.code_path,
                    self.project.sdk_path,
                    self.output_path .. "ExternalCode/",
                    self.output_path .. "ExternalSDKs/" })

                -- config links.
                if b.module.dependencies and next(b.module.dependencies) ~= nil then
                    b:links(b.module.dependencies)
                end
                if b.module.sdks and next(b.module.sdks) ~= nil then
                    b:links(b.module.sdks)
                end

                -- call case & platform callback.
                if type(b.case.on_config) == "function" then
                    b.case:on_config(b) 
                end
                if type(b.platform.on_config) == "function" then
                    b.platform:on_config(b)
                end
                -- call module callback.
                b.module:on_config(b)
            end
        end
    end

    sln.generate_solution = function(self, sln_type)
        if next(self.cases) == nil then
            error ("No case is added to the context.")
        end
        if next(self.platforms) == nil then
            error ("No platform is added to the context.")
        end
        self.output_path = lunafs.format_as_dir(self.output_path)
        local output_path_abs = self.project.path .. self.output_path
        lunafs.create_path(output_path_abs)
        lunafs.create_path(output_path_abs .. "ExternalCode/")
        lunafs.create_path(output_path_abs .. "ExternalSDKs/")

        for module_name, module in pairs(self.modules) do
            if module.belonging_project ~= self.project then
                local from_dir = module.belonging_project.path .. module.belonging_project.code_path .. module_name
                local to_dir = output_path_abs .. "ExternalCode/" .. module_name
                lunafs.copy_folder(from_dir, to_dir)
            end
        end

        for module_name, module in pairs(self.sdks) do
            if module.belonging_project ~= self.project then
                local from_dir = module.belonging_project.path .. module.belonging_project.sdk_path .. module_name
                local to_dir = output_path_abs .. "ExternalSDKs/" .. module_name
                lunafs.copy_folder(from_dir, to_dir)
            end
        end

        local b = {}
        b._solution = self
        -- internal attrs and methods.
        b._buf = ""
        b._append = function(self, str)
            self._buf = self._buf .. str
        end
        b._entry = function(self, name, value)
            self._buf = self._buf .. name .. " \"" .. value .. "\"\n"
        end
        b._table = function(self, name, values)
            self._buf = self._buf .. name .. " { "
            for _k, v in pairs(values) do
                self._buf = self._buf .. "\"" .. v .. "\", "
            end
            self._buf = self._buf .. "}\n"
        end

        -- public attrs and methods.
        b.includedirs = function(self, values)
            self:_append("includedirs {")
            for _k, v in pairs(values) do
                self:_append("\"" .. lunafs.create_relative_path(self._solution.project.path, self._solution.output_path) .. v .. "\", ")
            end
            self:_append("}\n")
        end
        b.defines = function(self, values)
            self:_table("defines", values)
        end
        b.undefines = function(self, values)
            self:_table("undefines", values)
        end
        b.libdirs = function(self, values)
            self:_append("libdirs {")
            for _k, v in pairs(values) do
                self:_append("\"" .. lunafs.create_relative_path(self._solution.project.path, self._solution.output_path) .. v .. "\", ")
            end
            self:_append("}\n")
        end
        b.links = function(self, values)
            self:_table("links", values)
        end
        b.kind = function(self, value)
            if value == "exe-console" then
                b:_entry("kind", "ConsoleApp")
            elseif value == "exe-windowed" then
                b:_entry("kind", "WindowedApp")
            elseif value == "dll" then
                b:_entry("kind", "SharedLib")
            elseif value == "lib" then
                b:_entry("kind", "StaticLib")
            elseif value == "utility" then
                b:_entry("kind", "Utility")
            elseif value == "none" then
                b:_entry("kind", "None")
            else
                error("Unsupported config.kind value: " .. config.kind)
            end
        end
        b.targetdir = function(self, value)
            self:_entry("targetdir", value)
        end
        b.files = function(self, values)
            self:_append("files {")
            for _k, v in pairs(values) do
                self:_append("\"" .. lunafs.create_relative_path(self._module_path, self._solution.output_path) .. v .. "\", ")
            end
            self:_append("}\n")
        end
        b.removefiles = function(self, values)
            self:_table("removefiles", values)
        end
        b.symbols = function(self, value)
            if value == "on" then
                self:_entry("symbols", "On")
            else
                self:_entry("symbols", "Off")
            end
        end
        b.optimize = function(self, value)
            if value == "full" then
                self:_entry("optimize", "Full")
            elseif value == "speed" then
                self:_entry("optimize", "Speed")
            elseif value == "off" then
                self:_entry("optimize", "Off")
            end
        end
        b.system = function(self, value)
            if value == "windows" then
                self:_entry("system", "windows")
            elseif value == "linux" then
                self:_entry("system", "linux")
            end
        end
        b.architecture = function(self, value)
            if value == "x86" then
                self:_entry("architecture", "x86")
            elseif value == "x86-64" then
                self:_entry("architecture", "x86_64")
            elseif value == "arm" then
                self:_entry("architecture", "ARM")
            end
        end
        b.compiler = function(self, value)
            if value == "msvc" then
                self:_entry("toolset", "msc")
            elseif value == "gcc" then
                self:_entry("toolset", "gcc")
            elseif value == "clang" then
                self:_entry("toolset", "clang")
            end
        end


        b:_entry("workspace", self.name)
        b.project = project
        b:_append("configurations {")
        for k, _v in pairs(self.cases) do
            b:_append("\"" .. k .. "\" , ")
        end
        b:_append("}\n")
        b:_append("platforms {")
        for k, _v in pairs(self.platforms) do
            b:_append("\"" .. k .. "\" , ")
        end
        b:_append("}\n")
        b:_table("flags", { "NoPCH" , "MultiProcessorCompile"})
        if sln_type == "vs2017" then
            b:_append("cppdialect \"C++latest\"\n")
        else
            b:_append("cppdialect \"C++17\"\n")
        end
        b:_append("systemversion \"latest\"\n")

        -- build modules.
        for module_name, module in pairs(self.modules) do
            if module.belonging_project == self.project then
                b:_entry("group", "Current")
                b._module_path = self.project.path .. self.project.code_path .. module_name .. "/"
            else
                b:_entry("group", "External")
                b._module_path = self.output_path .. "ExternalCode/" .. module_name .. "/"
            end
            self:_generate_module(b, module_name, module)
        end
        -- build sdks.
        for module_name, module in pairs(self.sdks) do
            if module.belonging_project == self.project then
                b:_entry("group", "SDKs")
                b._module_path = self.project.path .. self.project.sdk_path .. module_name .. "/"
            else
                b:_entry("group", "ExternalSDKs")
                b._module_path = self.output_path .. "ExternalSDKs/" .. module_name .. "/"
            end
            self:_generate_module(b, module_name, module)
        end
        local f = io.open(self.project.path .. self.output_path .. "premake5.lua", "w")
        f:write(b._buf)
        f:close()

        local execute_s = "premake5 --file=" .. self.project.path .. self.output_path .. "premake5.lua"
        if sln_type == "vs2015" then
            os.execute(execute_s .. " vs2015")
        elseif sln_type == "vs2017" then
            os.execute(execute_s .. " vs2017")
        elseif sln_type == "vs2019" then
            os.execute(execute_s .. " vs2019")
        elseif sln_type == "gmake" then
            os.execute(execute_s .. " gamke")
        elseif sln_type == "xcode" then
            os.execute(execute_s .. " xcode")
        end
    end

    return sln
end

lunamake.create_case = function(template, ...)
    template = template or "default"
    local c = require ("lunamake.templates.cases." .. template) -- load template package.
    return c.produce(...)
end

lunamake.create_platform = function(template, ...)
    template = template or "default"
    local c = require ("lunamake.templates.platforms." .. template) -- load template package.
    return c.produce(...)
end

return lunamake