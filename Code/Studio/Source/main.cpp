// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file main.cpp
* @author JXMaster
* @date 2020/4/20
*/
#include "StudioHeader.hpp"
#include "ProjectSelector.hpp"
#include "MainEditor.hpp"

using namespace luna;

void run_editor()
{
	input::init();
	gfx::init();
	renderer::init();
	rpack::init();
	image::init();
	font::init();
	imgui::init();
	asset::init();
	texture::init();
	obj_loader::init();
	scene::init();
	e3d::init();

	// Run project selector.
	auto project = select_project();
	if (failed(project))
	{
		return;
	}

	// Run main editor.
	editor::run_main_editor(project.get());

	return;
}

int main()
{
	luna::init();
	run_editor();
	luna::shutdown();
	return 0;
}