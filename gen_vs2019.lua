local lunamake = require "lunamake"

local sln = lunamake.new_solution("Luna", "./")

sln:add_case("Debug", lunamake.create_case("debug"))
sln:add_case("Profile", lunamake.create_case("profile"))
sln:add_case("Release", lunamake.create_case("release"))
sln:add_platform("x86", lunamake.create_platform("windows-x86"))
sln:add_platform("x64", lunamake.create_platform("windows-x86-64"))
sln:add_all_modules()

sln:generate_solution("vs2019")