// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file main.cpp
* @author JXMaster
* @date 2020/4/20
*/
#include "StudioHeader.hpp"
#include <Runtime/Runtime.hpp>
#include "ProjectSelector.hpp"
#include "MainEditor.hpp"

using namespace Luna;

void run_editor()
{
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

void set_current_dir_to_process_path()
{
	Path p = get_process_path();
	p.pop_back();
	luassert_always(succeeded(set_current_dir(p.encode().c_str())));
}

int main()
{
	luassert_always(succeeded(Luna::init()));
	set_current_dir_to_process_path();
	run_editor();
	Luna::close();
	return 0;
}