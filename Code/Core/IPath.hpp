// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IPath.hpp
* @author JXMaster
* @date 2019/12/13
 */
#pragma once
#include "IString.hpp"
#include "IName.hpp"
#include <Base/ICloneable.hpp>

namespace luna
{
	enum class EPathSeparator : uint32
	{
		//! Uses slash (/). 
		slash = 0,				
		//! Uses the system preferred separator (back-slash on Windows, slash on other platforms).
		system_preferred = 1,	
		//! Uses back-slash(\).
		back_slash = 2
	};

	enum class EPathFlag : uint32
	{
		none = 0x00,
		absolute = 0x01,	// The path is absolute, if this is not set, then the path is relative.
		diretory = 0x02,	// The path represents a directory, if this is not set, then the path presents a file.
	};

	enum class EPathComponent : uint32
	{
		none = 0x00,
		root = 0x01,
		nodes = 0x02,
		flags = 0x04,
		all = root | nodes | flags
	};

	//! @interface IPath
	//! Path is one kind of string that describes the location of one node in a hierarchical-based node tree, given that each
	//! node in the tree can be identified by a name string. A file path is used to identify a file or folder in the given 
	//! file system.
	//! 
	//! The `IPath` object is designed to be platform-independent and efficient to handle file path related operations.
	//! In implementation, the path object does not store the path string directly, but breaks it down to several parts, and stores
	//! each part independently. This makes path-related operations very fast and consumes less memory if you need to store lots of 
	//! paths.
	//! 
	//! One path is composed by the following parts:
	//! 1. The root name, which usually determines the domain of the path. For example, then volume symbol on Windows (like C:) is 
	//! one kind of root name.
	//! 2. The directory nodes that composes the path. For example, "C:\Games\MyGame\" has root name "C:" and two directory nodes 
	//! "Games" and "MyGame". In file object, every directory node as well as the root name is stored independently as one `IName` 
	//! object, and they are grouped into one array to form the path sequence.
	//! 3. The path flags, see `EPathFlag` for details. Basically, path object uses flags to determine if one path is absolute (if 
	//! if begins with one separator), and if one path represents a directory (if it ends with one separator). This flags are properly
	//! set when the path string gets parsed, but it may not be correct. For example, if you parse one path string that represents 
	//! a directory but does not ends with a separator, the `EPathFlag::directory` will not be set for that path. The path object will
	//! not use runtime system calls like `file_attribute` to determine if one path is valid or represents a directory, it is the user's
	//! responsibility to check it before using it.
	//! 
	//! `IPath` interface represents a immutable path, see `IMutPath` for a mutable path.
	struct IPath : public ICloneable
	{
		luiid("{a78071a0-b91b-4475-b3bb-5d207adf27cf}");
		
		//! Gets the path flags.
		virtual EPathFlag flags() = 0;
		
		//! Generates a string that represents the current path.
		//! @param[in] separator The separator format to use. Default is slash since it is well supported by all major platforms, 
		//! even on Windows.
		//! @param[in] has_root If the root name is appended to the path string.
		virtual P<IString> encode(EPathSeparator separator = EPathSeparator::slash, bool has_root = true) = 0;

		//! Gets the root name of the path.
		virtual P<IName> root_name() = 0;
		
		//! Gets the number of nodes in the path.
		virtual size_t count_nodes() = 0;

		//! Gets one node in the path. The index must be a valid index between [0, count_nodes()).
		virtual P<IName> node(size_t index) = 0;

		//! Gets the extension name of the path, that is, the name string after the last dot(.) character.
		//! The extension is always in lower case.
		//! Returns a null name "" if the path does not have an extension name.
		virtual P<IName> extension() = 0;

		//! Gets the hash code of this path.
		//! An empty path (without root name and nodes) is always hashed to `0`, every path that has exactly the same
		//! root name and nodes will be hashed to the same hash code. The hash code is cached in the object and is 
		//! efficient to read.
		virtual size_t hash_code() = 0;

		//! Compares if current path and the specified path is the same in the specified components.
		//! @param[in] rhs The path to compare with.
		//! @param[in] components_to_compare The components to compare, default is to compare all components
		//! and only `true` if all components of both paths are equal.
		virtual bool equal_to(IPath* rhs, EPathComponent components_to_compare = EPathComponent::all) = 0;

		//! Checks if the current path is the subsequent path of the specified base path. 
		//! For example, "/foo/bar" is a subsequent path of "/foo".
		//!
		//! @remark Path A is the subsequent path of B if:
		//! 1. Path A contains all nodes of B as its prefix path, in the same order as B.
		//! 2. If both paths have root names, their root names should be equal.
		//! The path flags (absolute/relative, file/directory) are ignored while checking.
		virtual bool is_subpath_of(IPath* base) = 0;
	};

	template <> struct Hash<IPath*>
	{
		size_t operator()(IPath* val) const { return val->hash_code(); }
	};

	template <> struct Hash<P<IPath>>
	{
		size_t operator()(const P<IPath>& val) const { return val->hash_code(); }
	};

	template <> struct EqualTo<IPath*>
	{
		bool operator()(IPath*& lhs, IPath*& rhs) const { return lhs->equal_to(rhs); }
	};

	template <> struct EqualTo<P<IPath>>
	{
		bool operator()(const P<IPath>& lhs, const P<IPath>& rhs) const { return lhs->equal_to(rhs); }
	};
}