// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IMutPath.hpp
* @author JXMaster
* @date 2019/12/13
*/
#pragma once
#include "IPath.hpp"

namespace luna
{
	//! @interface IMutPath
	//! Represents a mutable path.
	struct IMutPath : public IPath
	{
		luiid("{c45a78a8-f572-4a8c-93e8-e91c98ed527f}");

		//! Sets the path flags.
		virtual void set_flags(EPathFlag flags) = 0;

		//! Clears the current path and fills the path object by parsing the provided path string.
		//! The flags, root name and file nodes will be deduced from the string.
		//! 
		//! Currently only the following root string will be recognized:
		//! * Windows volume: C:, D: etc.
		//! * Remove Server: //My_Server or \\My_Server or IP-address format(\\192.168.31.1)
		virtual void parse(const char* path_string, size_t len = size_t_max_v) = 0;

		//! Replaces the extension of the original path.
		//! @param[in] new_extension The new extension to replace (without dot). If this is `nullptr`,
		//! the extension will be removed from the path.
		virtual void replace_extension(const char* new_extension, size_t len = size_t_max_v) = 0;

		//! Appends one extension to the original path.
		virtual void append_extension(const char* new_extension, size_t len = size_t_max_v) = 0;

		//! Sets the root name of the path.
		virtual void set_root_name(P<IName> root_path) = 0;

		//! Sets the node in the path. The index must be a valid index between [0, count_nodes()).
		virtual void set_node(size_t index, P<IName> node_string) = 0;

		//! Appends another path to the end of this path. The flags and root names of the appended path
		//! are ignored.
		virtual void append(IPath* appended_path) = 0;

		//! Appends one node to the end of the path.
		virtual void push_back(IName* node_string) = 0;

		//! Pops one node from the path. The path should have at least one node when this method is 
		//! called.
		virtual void pop_back() = 0;

		//! Erases `count` nodes starts from `pos`. The whole range to erase should be valid.
		virtual void erase(size_t pos, size_t count = 1) = 0;

		//! Clears all nodes in the path.
		virtual void clear() = 0;

		//! Assigns the content from `IPath` to this path.
		virtual void assign(IPath* from_path) = 0;

		//! Assigns the content of this path with a new relative path that if appended to `base` path, 
		//! creates a path equal to `target` path.
		//! @param[in] base The base path.
		//! @param[in] target The target path.
		//! @return If the system can decide how to redirect target path to this path, returns the relative path,
		//! else, returns failure.
		//! @remark In order to let this function succeeds, the following conditions are required:
		//! 1. The root name of this path and target path must be equal, either `nullptr` or points to the same `IName`
		//! object.
		//! 2. Both path should either all be absolute or all be relative.
		//! The returned path has the same root name as this path, and will always be a relative path. If this path is
		//! a directory, then the returned path will be marked as directory. The target path will always be recognized as
		//! directory, no matter if it is marked as directory.
		virtual RV assign_relative(IPath* base, IPath* target) = 0;
	};
}