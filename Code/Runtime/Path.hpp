// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Path.hpp
* @author JXMaster
* @date 2020/8/12
*/
#pragma once
#include "Name.hpp"
#include "Vector.hpp"
#include "Memory.hpp"

namespace Luna
{
	enum class EPathSeparator : u32
	{
		//! Uses slash (/). 
		slash = 0,
		//! Uses the system preferred separator (back-slash on Windows, slash on other platforms).
		system_preferred = 1,
		//! Uses back-slash(\).
		back_slash = 2
	};

	enum class EPathFlag : u32
	{
		none = 0x00,
		absolute = 0x01,	// The path is absolute, if this is not set, then the path is relative.
		diretory = 0x02,	// The path represents a directory, if this is not set, then the path presents a file.
	};

	enum class EPathComponent : u32
	{
		none = 0x00,
		root = 0x01,
		nodes = 0x02,
		flags = 0x04,
		all = root | nodes | flags
	};

	namespace PathImpl
	{
		inline bool is_separator(c8 ch)
		{
			return (ch == '\\' || ch == '/') ? true : false;
		}

		inline void get_path_root_name_length(const c8* path, usize slen, usize& len)
		{
			if (is_separator(path[0]) && slen >= 2)
			{
				if (is_separator(path[1]))
				{
					// Remote Computer.
					usize i = 2;
					while (path[i] && !is_separator(path[i]))
					{
						++i;
					}
					len = i;
					return;
				}
				else
				{
					// UNIX/Linux Root directory.
					len = 0;
					return;
				}
			}
			if (path[1] == ':' && isalpha(path[0]) && slen >= 2)
			{
				// Windows disk.
				len = 2;
				return;
			}
			len = 0;
		}

		inline c8 get_preferred_separator(EPathSeparator sep)
		{
			switch (sep)
			{
			case EPathSeparator::system_preferred:
#ifdef LUNA_PLATFORM_WINDOWS
				return '\\';
#else
				return '/';
#endif
			case EPathSeparator::back_slash:
				return '\\';
			case EPathSeparator::slash:
			default:
				return '/';
			}
		}

		//! Fetches the next node from the string. The node ends with a separator or end of the string.
		inline usize get_next_node(const c8* p, usize len)
		{
			usize i = 0;
			while ((i < len) && !is_separator(p[i]))
			{
				++i;
			}
			return i;
		}
	}

	//! @class Path
	//! Path is one kind of string that describes the location of one node in a hierarchical-based node tree, given that each
	//! node in the tree can be identified by a name string. A file path is used to identify a file or folder in the given 
	//! file system.
	//! 
	//! The Path class is designed to be platform-independent and efficient to handle file path related operations.
	//! In implementation, the path object does not store the path string directly, but breaks it down to several parts, and stores
	//! each part independently. This makes path-related operations very fast and consumes less memory if you need to store lots of 
	//! paths.
	//! 
	//! One path is composed by the following parts:
	//! 1. The root name, which usually determines the domain of the path. For example, then volume symbol on Windows (like C:) is 
	//! one kind of root name.
	//! 2. The directory nodes that composes the path. For example, "C:\Games\MyGame\" has root name "C:" and two directory nodes 
	//! "Games" and "MyGame". In file object, every directory node as well as the root name is stored independently as one Name  
	//! object, and they are grouped into one array to form the path sequence.
	//! 3. The path flags, see `EPathFlag` for details. Basically, path object uses flags to determine if one path is absolute 
	//! (if begins with one separator), and if one path represents a directory (if it ends with one separator). This flags are properly
	//! set when the path string gets parsed, but it may not be correct. For example, if you parse one path string that represents 
	//! a directory but does not ends with a separator, the `EPathFlag::directory` will not be set for that path. The path object will
	//! not use runtime system calls like `file_attribute` to determine if one path is valid or represents a directory, it is the user's
	//! responsibility to check it before using it.
	class Path
	{
		Vector<Name> m_nodes;
		Name m_root;
		EPathFlag m_flags;

	public:

		using reference = Name&;
		using const_reference = const Name&;
		using pointer = Name*;
		using const_pointer = const Name*;
		using iterator = pointer;
		using const_iterator = const_pointer;
		using reverse_iterator = ReverseIterator<iterator>;
		using const_reverse_iterator = ReverseIterator<const_iterator>;

		Path() :
			m_flags(EPathFlag::none) {}

		Path(const String& str) :
			m_flags(EPathFlag::none)
		{
			assign(str);
		}
		Path(const String& str, usize pos) :
			m_flags(EPathFlag::none)
		{
			assign(str, pos);
		}
		Path(const String& str, usize pos, usize count) :
			m_flags(EPathFlag::none)
		{
			assign(str, pos, count);
		}
		Path(const c8* s) :
			m_flags(EPathFlag::none)
		{
			assign(s);
		}
		Path(const c8* s, usize count) :
			m_flags(EPathFlag::none)
		{
			assign(s, count);
		}
		Path(const Path& rhs) :
			m_flags(EPathFlag::none)
		{
			assign(rhs);
		}
		Path(Path&& rhs) :
			m_nodes(move(rhs.m_nodes)),
			m_flags(rhs.m_flags),
			m_root(move(rhs.m_root))
		{
			rhs.m_flags = EPathFlag::none;
		}

		Path& operator=(const String& str)
		{
			assign(str);
			return *this;
		}
		Path& operator=(const c8* s)
		{
			assign(s);
			return *this;
		}
		Path& operator=(const Path& rhs)
		{
			assign(rhs);
			return *this;
		}
		Path& operator=(Path&& rhs)
		{
			assign(move(rhs));
			return *this;
		}
		
		//! Retrieve the path flags.
		EPathFlag flags() const
		{
			return m_flags;
		}
		EPathFlag& flags()
		{
			return m_flags;
		}

		//! Normalizes the path. 
		//! This call remove all unneeded ".." and "." nodes from the path.
		void normalize()
		{
			auto iter = m_nodes.begin();
			while (iter != m_nodes.end())
			{
				if ((!strcmp((*iter).c_str(), "..")) && (iter != m_nodes.begin()))
				{
					auto iter2 = iter;
					--iter2;
					if (strcmp((*iter2).c_str(), ".."))
					{
						iter = m_nodes.erase(iter2, iter2 + 2);
					}
				}
				else if ((!strcmp((*iter).c_str(), ".")))
				{
					iter = m_nodes.erase(iter, iter + 1);
				}
				else
				{
					++iter;
				}
			}
		}

		//! Generates a string that represents the current path.
		//! @param[in] separator The separator format to use. Default is slash since it is well supported by all major platforms, 
		//! even on Windows.
		//! @param[in] has_root If the root name is appended to the path string.
		String encode(EPathSeparator separator = EPathSeparator::slash, bool has_root = true) const
		{
			String buf;
			if (m_root && has_root)
			{
				buf.append(m_root.c_str());
			}
			// Append '/' for root.
			if ((flags() & EPathFlag::absolute) != EPathFlag::none)
			{
				buf.push_back(PathImpl::get_preferred_separator(separator));
			}
			// Append '.' if not root and is empty.
			else if (m_nodes.empty())
			{
				buf.push_back('.');
			}
			for (usize i = 0; i < m_nodes.size(); ++i)
			{
				buf.append(m_nodes[i].c_str());
				if (i != m_nodes.size() - 1)
				{
					buf.push_back(PathImpl::get_preferred_separator(separator));
				}
			}
			// Append '/'(or any preferred separator) at the end for directory if it is not ended with '/'.
			if (((flags() & EPathFlag::diretory) != EPathFlag::none) && (!buf.empty()) &&
				(buf.c_str()[buf.size() - 1] != PathImpl::get_preferred_separator(separator)))
			{
				buf.push_back(PathImpl::get_preferred_separator(separator));
			}
			return buf;
		}

		//! Assigns another path to this path.
		void assign(const Path& rhs)
		{
			m_flags = rhs.m_flags;
			m_nodes = rhs.m_nodes;
			m_root = rhs.m_root;
		}
		void assign(Path&& rhs)
		{
			m_flags = rhs.m_flags;
			m_nodes = move(rhs.m_nodes);
			m_root = move(m_root);
			rhs.m_flags = EPathFlag::none;
		}

		//! Clears the current path and fills the path object by parsing the provided path string.
		//! The flags, root name and file nodes will be deduced from the string.
		//! 
		//! Currently only the following root string will be recognized:
		//! * Windows volume: C:, D: etc.
		//! * Remove Server: //My_Server or \\My_Server or IP-address format(\\192.168.31.1)
		void assign(const String& str)
		{
			assign(str.c_str(), str.size());
		}
		void assign(const String& str, usize pos)
		{
			assign(str.c_str() + pos, str.size() - pos);
		}
		void assign(const String& str, usize pos, usize count)
		{
			assign(str.c_str() + pos, count);
		}
		void assign(const c8* s)
		{
			assign(s, strlen(s));
		}
		void assign(const c8* s, usize count)
		{
			m_nodes.clear();
			usize cur = 0;
			// Parse root path.
			usize rl;
			PathImpl::get_path_root_name_length(s, count, rl);
			if (rl)
			{
				m_root = Name(s, rl);
			}
			else
			{
				m_root = nullptr;
			}
			cur += rl;
			// Check absolute path.
			m_flags = EPathFlag::none;
			if (PathImpl::is_separator(s[cur]))
			{
				m_flags |= EPathFlag::absolute;
				cur += 1;
			}
			else
			{
				m_flags &= ~(EPathFlag::absolute);
			}
			// Parse nodes.
			while (cur < count)
			{
				usize i = PathImpl::get_next_node(s + cur, count - cur);
				if (!i)
				{
					++cur;
					continue;
				}
				m_nodes.push_back(Name(s + cur, i));
				cur += i + 1;
			}
			if (PathImpl::is_separator(s[count - 1]))
			{
				m_flags |= EPathFlag::diretory;
			}
			else
			{
				m_flags &= ~(EPathFlag::diretory);
			}
			normalize();
		}

		//! Assigns the content of this path with a new relative path that if appended to `base` path, 
		//! creates a path equal to `target` path.
		//! @param[in] base The base path.
		//! @param[in] target The target path.
		//! @return If the system can decide how to redirect target path to this path, returns the relative path,
		//! else, returns failure.
		//! @remark In order to let this function succeeds, the following conditions are required:
		//! 1. The root name of this path and target path must be equal, either `nullptr` or points to the same `Name`
		//! object.
		//! 2. Both path should either all be absolute or all be relative.
		//! The returned path has the same root name as this path, and will always be a relative path. If this path is
		//! a directory, then the returned path will be marked as directory. The target path will always be recognized as
		//! directory, no matter if it is marked as directory.
		void assign_relative(const Path& base, const Path& target)
		{
			luassert_msg(base.root() == target.root(), "The root name for base and target path must be equal.");
			luassert_msg((base.flags() & EPathFlag::absolute) == (target.flags() & EPathFlag::absolute), "The base and target path must all be absolute or relative");
			clear();
			// Finds the common prefix.
			usize diff_begin;
			usize nodes = min(base.size(), target.size());
			for (diff_begin = 0; diff_begin < nodes; ++diff_begin)
			{
				if (base[diff_begin] != target[diff_begin])
				{
					break;
				}
			}
			if ((target.flags() & EPathFlag::diretory) != EPathFlag::none)
			{
				m_flags = EPathFlag::diretory;
			}
			else
			{
				m_flags = EPathFlag::none;
			}
			m_root = base.root();
			auto dd = Name("..");
			for (usize i = diff_begin; i < base.size(); ++i)
			{
				m_nodes.push_back(dd);
			}
			dd = nullptr;
			for (usize i = diff_begin; i < target.size(); ++i)
			{
				m_nodes.push_back(target[i]);
			}
		}

		//! Retrieve the path root name.
		const Name& root() const
		{
			return m_root;
		}
		Name& root()
		{
			return m_root;
		}

		//! Gets the extension name of the path, that is, the name string after the last dot(.) character.
		//! The extension is always in lower case.
		//! Returns a null Name ("") if the path does not have an extension name.
		Name extension() const
		{
			if (m_nodes.empty())
			{
				return Name("");
			}
			auto& name = m_nodes.back();
			const c8* str = name.c_str();
			usize sz = strlen(str);
			usize i = sz - 1;	// points to the last valid char.
			while (i)
			{
				if (str[i] == '.')
				{
					// translate to lower case.
					usize ext_sz = sz - i;
					if (ext_sz == 1)
					{
						return Name("");
					}
					c8* buf = (c8*)alloca(sizeof(c8) * ext_sz);
					memcpy(buf, str + i + 1, ext_sz * sizeof(c8));
					for (usize j = 0; j < ext_sz - 1; ++j)
					{
						buf[j] = (c8)tolower(buf[j]);
					}
					return Name(buf);
				}
				--i;
			}
			return Name("");
		}
		void replace_extension(const c8* new_extension)
		{
			if (new_extension)
			{
				replace_extension(new_extension, strlen(new_extension));
			}
			else
			{
				replace_extension(new_extension, 0);
			}
		}
		void replace_extension(const c8* new_extension, usize count)
		{
			auto& name = m_nodes.back();
			const c8* str = name.c_str();
			usize sz = strlen(str);
			usize i = sz - 1;	// points to the last valid char.
			// Finds the length of the extension.
			while (i)
			{
				if (str[i] == '.')
				{
					break;
				}
				--i;
			}
			usize filename_len = i;
			usize new_filename_len;
			if (new_extension)
			{
				new_filename_len = count + filename_len + 1;	// 1 for dot(.)
			}
			else
			{
				new_filename_len = filename_len;
			}
			c8* buf = (c8*)alloca(sizeof(c8) * (new_filename_len + 1));
			buf[new_filename_len] = 0;
			// copy filename.
			memcpy(buf, str, filename_len * sizeof(c8));
			// copy extension.
			if (new_extension)
			{
				buf[filename_len] = '.';
				memcpy(buf + filename_len + 1, new_extension, count * sizeof(c8));
				for (usize i = 0; i < count; ++i)
				{
					buf[filename_len + 1 + i] = (c8)tolower(buf[filename_len + 1 + i]);
				}
			}
			name = Name(buf);;
		}
		void append_extension(const c8* new_extension)
		{
			append_extension(new_extension, strlen(new_extension));
		}
		void append_extension(const c8* new_extension, usize count)
		{
			auto& name = m_nodes.back();
			const c8* str = name.c_str();
			usize sz = strlen(str);
			c8* buf = (c8*)alloca(sizeof(c8) * (sz + count + 2));
			// copy original namec8
			memcpy(buf, str, sz * sizeof(c8));
			buf[sz] = '.';
			// copy extension.
			memcpy(buf + sz + 1, new_extension, count * sizeof(c8));
			// ends with NULL.
			buf[sz + count + 1] = 0;
			name = Name(buf);
		}

		const_reference at(usize index) const
		{
			return m_nodes[index];
		}
		reference at(usize index)
		{
			return m_nodes[index];
		}
		const_reference operator[](usize index) const
		{
			return at(index);
		}
		reference operator[](usize index)
		{
			return at(index);
		}

		iterator begin()
		{
			return m_nodes.begin();
		}
		const_iterator begin() const
		{
			return m_nodes.begin();
		}
		const_iterator cbegin() const
		{
			return m_nodes.cbegin();
		}
		iterator end()
		{
			return m_nodes.end();
		}
		const_iterator end() const
		{
			return m_nodes.end();
		}
		const_iterator cend() const
		{
			return m_nodes.cend();
		}

		reverse_iterator rbegin()
		{
			return m_nodes.rbegin();
		}
		const_reverse_iterator rbegin() const
		{
			return m_nodes.rbegin();
		}
		const_reverse_iterator crbegin() const
		{
			return m_nodes.crbegin();
		}
		reverse_iterator rend()
		{
			return m_nodes.rend();
		}
		const_reverse_iterator rend() const
		{
			return m_nodes.rend();
		}
		const_reverse_iterator crend() const
		{
			return m_nodes.crend();
		}

		usize size() const
		{
			return m_nodes.size();
		}
		bool empty() const
		{
			return m_nodes.empty();
		}

		reference front()
		{
			return m_nodes.front();
		}
		const_reference front() const
		{
			return m_nodes.front();
		}
		reference back()
		{
			return m_nodes.back();
		}
		const_reference back() const
		{
			return m_nodes.back();
		}

		//! Appends one node to the end of the path.
		void push_back(const Name& path_node)
		{
			m_nodes.push_back(path_node);
		}
		void push_back(Name&& path_node)
		{
			m_nodes.push_back(move(path_node));
		}

		//! Pops one node from the path. The path should have at least one node when this method is 
		//! called.
		void pop_back()
		{
			m_nodes.pop_back();
		}

		//! Appends another path to the end of this path. The flags and the root name of the appended path
		//! are ignored.
		void append(const Path& appended_path)
		{
			for (auto& i : appended_path.m_nodes)
			{
				m_nodes.push_back(i);
			}
		}
		void append(const Path& appended_path, usize pos)
		{
			for (auto iter = appended_path.m_nodes.begin() + pos; iter != appended_path.m_nodes.end(); ++iter)
			{
				m_nodes.push_back(*iter);
			}
		}
		void append(const Path& appended_path, usize pos, usize count)
		{
			for (usize i = 0; i < count; ++i)
			{
				m_nodes.push_back(appended_path.m_nodes[pos + i]);
			}
		}

		//! Clears all nodes in the path.
		void clear()
		{
			m_nodes.clear();
		}

		//! Erases `count` nodes starts from `pos`. The whole range to erase should be valid.
		void erase(usize pos, usize count)
		{
			m_nodes.erase(m_nodes.begin() + pos, m_nodes.begin() + pos + count);
		}
		//! Erases the node identified by the iterator.
		iterator erase(const_iterator pos)
		{
			return m_nodes.erase(pos);
		}
		//! Erases elements in range identified by iterators.
		iterator erase(const_iterator first, const_iterator last)
		{
			return m_nodes.erase(first, last);
		}

		//! Gets the hash code of this path.
		usize hash_code() const
		{
			usize h = ((m_flags & EPathFlag::absolute) == EPathFlag::none) ? 0 : 0x3745; // Random initial seed to To deferent "/A/B" from "A/B".
			u64 id = 0;
			if (m_root)
			{
				id = m_root.id();
				h = memhash<usize>(&id, sizeof(u64), h);
				h = strhash<usize>("://", h);// To deferent "A://B" from "/A/B"
			}
			for (auto& i : m_nodes)
			{
				id = i.id();
				h = memhash<usize>(&id, sizeof(u64), h);
			}
			return h;
		}

		//! Checks if the current path is the subsequent path of the specified base path. 
		//! For example, "/foo/bar" is a subsequent path of "/foo".
		//!
		//! @remark Path A is the subsequent path of B if:
		//! 1. Path A contains all nodes of B as its prefix path, in the same order as B.
		//! 2. If both paths have root names, their root names should be identical.
		//! The path flags (absolute/relative, file/directory) are ignored while checking.
		bool is_subpath_of(const Path& base) const
		{
			auto base_root = base.root();
			if (m_root && base_root && (m_root != base_root))
			{
				return false;
			}
			if (m_nodes.size() < base.size())
			{
				return false;
			}
			for (usize i = 0; i < base.size(); ++i)
			{
				if (m_nodes[i] != base[i])
				{
					return false;
				}
			}
			return true;
		}

		//! Compares if current path and the specified path is the same in the specified components.
		//! @param[in] rhs The path to compare with.
		//! @param[in] components_to_compare The components to compare, default is to compare all components
		//! and only `true` if all components of both paths are equal.
		bool equal_to(const Path& rhs, EPathComponent components_to_compare = EPathComponent::all) const
		{
			if ((components_to_compare & EPathComponent::flags) != EPathComponent::none)
			{
				if (m_flags != rhs.m_flags)
				{
					return false;
				}
			}
			if ((components_to_compare & EPathComponent::root) != EPathComponent::none)
			{
				if (m_root != rhs.m_root)
				{
					return false;
				}
			}
			if ((components_to_compare & EPathComponent::nodes) != EPathComponent::none)
			{
				if (m_nodes.size() != rhs.m_nodes.size())
				{
					return false;
				}
				for (usize i = 0; i < m_nodes.size(); ++i)
				{
					if (m_nodes[i] != rhs.m_nodes[i])
					{
						return false;
					}
				}
			}
			return true;
		}

		//! Same as `equal_to` with `components_to_compare` set to `EPathComponent::all`.
		bool operator==(const Path& rhs) const
		{
			return equal_to(rhs);
		}
		bool operator!=(const Path& rhs) const
		{
			return !equal_to(rhs);
		}
	};

	template <> struct hash<Path>
	{
		usize operator()(const Path& val) const { return val.hash_code(); }
	};
}
