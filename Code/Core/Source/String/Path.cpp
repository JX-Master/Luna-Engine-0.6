// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Path.cpp
* @author JXMaster
* @date 2019/12/18
 */
#include "Path.hpp"
namespace luna
{
	inline bool is_separator(char ch)
	{
		return (ch == '\\' || ch == '/') ? true : false;
	}

	inline void get_path_root_name_length(const char* path, size_t slen, size_t& len)
	{
		if (is_separator(path[0]) && slen >= 2)
		{
			if (is_separator(path[1]))
			{
				// Remote Computer.
				size_t i = 2;
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

	inline char get_preferred_separator(EPathSeparator sep)
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
	inline size_t get_next_node(const char* p, size_t len)
	{
		size_t i = 0;
		while ((i < len) && !is_separator(p[i]))
		{
			++i;
		}
		return i;
	}

	
	void Path::normalize_path()
	{
		auto iter = m_nodes.begin();
		while (iter != m_nodes.end())
		{
			if ((!strcmp((*iter)->c_str(), "..")) && (iter != m_nodes.begin()))
			{
				auto iter2 = iter;
				--iter2;
				if (strcmp((*iter2)->c_str(), ".."))
				{
					iter = m_nodes.erase(iter2, iter2 + 2);
				}
			}
			else if ((!strcmp((*iter)->c_str(), ".")))
			{
				iter = m_nodes.erase(iter, iter + 1);
			}
			else
			{
				++iter;
			}
		}
	}

	P<IString> Path::encode(EPathSeparator separator, bool has_root)
	{
		lutsassert();
		P<IStringBuffer> buf = new_string_buffer();
		if (m_root && has_root)
		{
			buf->append(m_root->c_str());
		}
		// Append '/' for root.
		if((flags() & EPathFlag::absolute) != EPathFlag::none)
		{
			buf->push_back(get_preferred_separator(separator));
		}
		// Append '.' if not root and is empty.
		else if (m_nodes.empty())
		{
			buf->push_back('.');
		}
		for (size_t i = 0; i < m_nodes.size(); ++i)
		{
			buf->append(m_nodes[i]->c_str());
			if (i != m_nodes.size() - 1)
			{
				buf->push_back(get_preferred_separator(separator));
			}
		}
		// Append '/'(or any preferred separator) at the end for directory if it is not ended with '/'.
		if (((flags() & EPathFlag::diretory) != EPathFlag::none) && (!buf->empty()) && 
			(buf->c_str()[buf->size() - 1] != get_preferred_separator(separator)))
		{
			buf->push_back(get_preferred_separator(separator));
		}
		return buf;
	}

	void Path::parse(const char* path_string, size_t len)
	{
		luassert_usr(path_string && len);
		lutsassert();
		reset_hash();
		m_nodes.clear();
		if (len == size_t_max_v)
		{
			len = strlen(path_string);
		}
		size_t cur = 0;
		// Parse root path.
		size_t rl;
		get_path_root_name_length(path_string, len, rl);
		if (rl)
		{
			m_root = intern_name(path_string, rl);
		}
		else
		{
			m_root = nullptr;
		}
		cur += rl;
		// Check absolute path.
		set_flags(EPathFlag::none);
		if (is_separator(path_string[cur]))
		{
			set_flags(flags() | EPathFlag::absolute);
			cur += 1;
		}
		else
		{
			set_flags(flags() & ~(EPathFlag::absolute));
		}
		// Parse nodes.
		while (cur < len)
		{
			size_t i = get_next_node(path_string + cur, len - cur);
			if (!i)
			{
				++cur;
				continue;
			}
			auto path_cur = intern_name(path_string + cur, i);
			m_nodes.push_back(path_cur);
			cur += i + 1;
		}
		if (is_separator(path_string[len - 1]))
		{
			set_flags(flags() | EPathFlag::diretory);
		}
		else
		{
			set_flags(flags() & ~(EPathFlag::diretory));
		}
		normalize_path();
	}

	void Path::replace_extension(const char* new_extension, size_t len)
	{
		lutsassert();
		auto& name = m_nodes.back();
		size_t i = name->size() - 1;	// points to the last valid char.
		// Finds the length of the extension.
		while (i)
		{
			if (name->c_str()[i] == '.')
			{
				break;
			}
			--i;
		}
		size_t filename_len = i;
		size_t new_filename_len;
		if (new_extension)
		{
			if (len == size_t_max_v)
			{
				len = strlen(new_extension);
			}
			new_filename_len = len + filename_len + 1;	// 1 for dot(.)
		}
		else
		{
			new_filename_len = filename_len;
		}
		char* buf = (char*)alloca(sizeof(char) * (new_filename_len + 1));
		buf[new_filename_len] = 0;
		// copy filename.
		memcpy(buf, name->c_str(), filename_len * sizeof(char));
		// copy extension.
		if (new_extension)
		{
			buf[filename_len] = '.';
			memcpy(buf + filename_len + 1, new_extension, len * sizeof(char));
			for (size_t i = 0; i < len; ++i)
			{
				buf[filename_len + 1 + i] = (char)tolower(buf[filename_len + 1 + i]);
			}
		}

		auto new_filename = intern_name(buf);
		name = new_filename;
	}

	void Path::append_extension(const char* new_extension, size_t len)
	{
		luassert_usr(new_extension);
		lutsassert();
		if (len == size_t_max_v)
		{
			len = strlen(new_extension);
		}
		auto& name = m_nodes.back();
		char* buf = (char*)alloca(sizeof(char) * (name->size() + len + 2));
		// copy original name.
		memcpy(buf, name->c_str(), name->size() * sizeof(char));
		buf[name->size()] = '.';
		// copy extension.
		memcpy(buf + name->size() + 1, new_extension, len * sizeof(char));
		// ends with NULL.
		buf[name->size() + len + 1] = 0;
		auto new_filename = intern_name(buf);
		name = new_filename;
	}

	void Path::assign(IPath* from_path)
	{
		luassert_usr(from_path);
		lutsassert();
		clear();
		m_flags = from_path->flags();
		m_root = from_path->root_name();
		append(from_path);
	}

	bool Path::equal_to(IPath* rhs, EPathComponent components_to_compare)
	{
		lutsassert();
		Path* p = static_cast<Path*>(rhs);
		if ((components_to_compare & EPathComponent::flags) != EPathComponent::none)
		{
			if (m_flags != p->m_flags)
			{
				return false;
			}
		}
		if ((components_to_compare & EPathComponent::root) != EPathComponent::none)
		{
			if (m_root != p->m_root)
			{
				return false;
			}
		}
		if ((components_to_compare & EPathComponent::nodes) != EPathComponent::none)
		{
			if (m_nodes.size() != p->m_nodes.size())
			{
				return false;
			}
			for (size_t i = 0; i < m_nodes.size(); ++i)
			{
				if (m_nodes[i] != p->m_nodes[i])
				{
					return false;
				}
			}
		}
		return true;
	}

	bool Path::is_subpath_of(IPath* base)
	{
		luassert_usr(base);
		lutsassert();
		auto base_root = base->root_name();
		if (m_root && base_root && (m_root != base_root))
		{
			return false;
		}
		if (m_nodes.size() < base->count_nodes())
		{
			return false;
		}
		for (size_t i = 0; i < base->count_nodes(); ++i)
		{
			if (m_nodes[i] != base->node(i))
			{
				return false;
			}
		}
		return true;
	}

	RV Path::assign_relative(IPath* base, IPath* target)
	{
		luassert_usr(base && target);
		lutsassert();
		clear();
		if (base->root_name() != target->root_name())
		{
			return e_bad_arguments;
		}
		if ((base->flags() & EPathFlag::absolute) !=
			(target->flags() & EPathFlag::absolute))
		{
			return e_bad_arguments;
		}
		// Finds the common prefix.
		size_t diff_begin;
		size_t nodes = min(base->count_nodes(), target->count_nodes());
		for (diff_begin = 0; diff_begin < nodes; ++diff_begin)
		{
			if (base->node(diff_begin) != target->node(diff_begin))
			{
				break;
			}
		}
		if ((target->flags() & EPathFlag::diretory) != EPathFlag::none)
		{
			set_flags(EPathFlag::diretory);
		}
		else
		{
			set_flags(EPathFlag::none);
		}
		set_root_name(base->root_name());
		auto dd = intern_name("..");
		for (size_t i = diff_begin; i < base->count_nodes(); ++i)
		{
			m_nodes.push_back(dd);
		}
		dd = nullptr;
		for (size_t i = diff_begin; i < target->count_nodes(); ++i)
		{
			m_nodes.push_back(target->node(i));
		}
		return s_ok;
	}
}