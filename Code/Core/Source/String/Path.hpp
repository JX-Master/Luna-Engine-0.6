// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Path.hpp
* @author JXMaster
* @date 2019/12/17
 */
#pragma once
#include "../CoreHeader.hpp"
#include <Base/Vector.hpp>
#include <Base/TSAssert.hpp>
namespace luna
{
	class Path : public IMutPath
	{
	public:
		lucid("{09b3d149-4096-425e-a4c9-42d4d9867cce}");
		luiimpl(Path, IMutPath, IPath, ICloneable, IObject);
		lutsassert_lock();

		// lower 16 bits: EPathFlag
		// higher 16 bits: EPathSeparator
		EPathFlag m_flags;
		P<IName> m_root;
		size_t m_hash;
		Vector<P<IName>> m_nodes;

		Path(IAllocator* alloc) :
			luibind(alloc),
			m_nodes(alloc),
			m_hash(0),
			m_flags(EPathFlag::none) {}

		// remove all unneeded ".." and "."
		void normalize_path();

		void reset_hash()
		{
			m_hash = 0;
		}

		virtual P<IObject> clone() override
		{
			P<IMutPath> p = new_path();
			p->assign((IPath*)this);
			return p;
		}

		virtual EPathFlag flags() override
		{
			lutsassert();
			return m_flags;
		}
		virtual void set_flags(EPathFlag flags) override
		{
			lutsassert();
			m_flags = flags;
		}

		virtual P<IString> encode(EPathSeparator separator = EPathSeparator::slash, bool has_root = true) override;
		virtual void parse(const char* path_string, size_t len = size_t_max_v) override;
		virtual void replace_extension(const char* new_extension, size_t len = size_t_max_v) override;
		virtual void append_extension(const char* new_extension, size_t len = size_t_max_v) override;
		virtual P<IName> root_name() override
		{
			return m_root;
		}
		virtual void set_root_name(P<IName> root_path) override
		{
			lutsassert();
			reset_hash();
			m_root = root_path;
		}

		virtual size_t count_nodes() override
		{
			return m_nodes.size();
		}
		virtual P<IName> node(size_t index) override
		{
			lutsassert();
			luassert_usr(index <= m_nodes.size());
			return m_nodes[index];
		}
		virtual void set_node(size_t index, P<IName> node_string) override
		{
			lutsassert();
			luassert_usr(node_string && (index <= m_nodes.size()));
			reset_hash();
			m_nodes[index] = node_string;
			normalize_path();
		}
		virtual P<IName> extension() override
		{
			lutsassert();
			if (m_nodes.empty())
			{
				return intern_name("");
			}
			auto& name = m_nodes.back();
			size_t i = name->size() - 1;	// points to the last valid char.
			while (i)
			{
				if (name->c_str()[i] == '.')
				{
					// translate to lower case.
					size_t ext_sz = name->size() - i;
					if (ext_sz == 1)
					{
						return intern_name("");
					}
					char* buf = (char*)alloca(sizeof(char) * ext_sz);
					strcpy_s(buf, ext_sz, name->c_str() + i + 1);
					for (size_t j = 0; j < ext_sz - 1; ++j)
					{
						buf[j] = (char)tolower(buf[j]);
					}
					return intern_name(buf);
				}
				--i;
			}
			return intern_name("");
		}
		virtual void push_back(IName* node_string) override
		{
			lutsassert();
			luassert_usr(node_string);
			m_nodes.push_back(node_string);
			reset_hash();
			normalize_path();
		}
		virtual void append(IPath* appended_path) override
		{
			lutsassert();
			luassert_usr(appended_path);
			reset_hash();
			for (size_t i = 0; i < appended_path->count_nodes(); ++i)
			{
				push_back(appended_path->node(i));
			}
			normalize_path();
		}
		virtual void pop_back() override
		{
			lutsassert();
			luassert_usr(!m_nodes.empty());
			reset_hash();
			m_nodes.pop_back();
		}
		virtual void erase(size_t pos, size_t count) override
		{
			lutsassert();
			luassert_usr(pos + count < m_nodes.size());
			reset_hash();
			m_nodes.erase(m_nodes.begin() + pos, m_nodes.begin() + pos + count);
		}
		virtual void clear() override
		{
			lutsassert();
			reset_hash();
			m_nodes.clear();
		}

		virtual void assign(IPath* from_path) override;
		virtual size_t hash_code() override
		{
			// All modifications to the path nodes and the root name will reset this to 0.
			if (!m_hash && (m_root || !m_nodes.empty()))	
			{
				if (m_root)
				{
					m_hash = memhash<size_t>(m_root.get_address_of(), sizeof(P<IName>), 0);
				}
				if (!m_nodes.empty())
				{
					m_hash = memhash<size_t>(m_nodes.data(), m_nodes.size() * sizeof(P<IName>), m_hash);
				}
			}
			return m_hash;
		}
		virtual bool equal_to(IPath* rhs, EPathComponent components_to_compare = EPathComponent::all) override;
		virtual bool is_subpath_of(IPath* base) override;
		virtual RV assign_relative(IPath* base, IPath* target) override;
	};

	template <> struct Hash<Path*>
	{
		size_t operator()(Path* val) const { return val->hash_code(); }
	};

	template <> struct Hash<P<Path>>
	{
		size_t operator()(P<Path> val) const { return val->hash_code(); }
	};
}