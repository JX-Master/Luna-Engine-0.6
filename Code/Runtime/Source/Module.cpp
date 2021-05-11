// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Name.cpp
* @author JXMaster
* @date 2020/8/8
*/
#include <Runtime/PlatformDefines.hpp>
#define LUNA_RUNTIME_API LUNA_EXPORT
#include "Module.hpp"
#include "../Module.hpp"
#include "../Assert.hpp"
#include "../Vector.hpp"
#include "../HashMap.hpp"
#include "../Name.hpp"
#include "../Result.hpp"

namespace Luna
{
	struct ModuleEntry
	{
		ModuleDesc* m_prior = nullptr;
		ModuleDesc* m_next = nullptr;
		bool m_initialized = false;
		bool m_resolved = false;
	};
	static_assert(sizeof(ModuleEntry) <= sizeof(c8) * 32, "ModuleEntry size too big!");

	ModuleDesc* g_module_header = nullptr;

	Unconstructed<Vector<ModuleDesc*>> g_module_init_queue;

	LUNA_RUNTIME_API void add_engine_module(ModuleDesc* module_desc)
	{
		ModuleEntry* entry = (ModuleEntry*)(module_desc->reserved);
		new (entry) ModuleEntry();
		entry->m_next = g_module_header;
		ModuleEntry* last_entry = (ModuleEntry*)(g_module_header->reserved);
		if (last_entry)
		{
			last_entry->m_prior = module_desc;
		}
		g_module_header = module_desc;
	}

	LUNA_RUNTIME_API void remove_engine_module(ModuleDesc* module_desc)
	{
		ModuleEntry* entry = (ModuleEntry*)(module_desc->reserved);
		ModuleDesc* prior = entry->m_prior;
		ModuleDesc* next = entry->m_next;
		if (prior)
		{
			ModuleEntry* prior_entry = (ModuleEntry*)(prior->reserved);
			prior_entry->m_next = next;
		}
		if (next)
		{
			ModuleEntry* next_extry = (ModuleEntry*)(next->reserved);
			next_extry->m_prior = prior;
		}
		entry->m_prior = nullptr;
		entry->m_next = nullptr;
		if (g_module_header == module_desc)
		{
			g_module_header = next;
		}
	}

	RV module_init()
	{
		g_module_init_queue.construct();
		auto& init_queue = g_module_init_queue.get();

		auto r = init_modules();

		if (!r.valid())
		{
			// Close all initialized module in reverse order.
			for (auto iter = init_queue.rbegin(); iter != init_queue.rend(); ++iter)
			{
				ModuleEntry* entry = (ModuleEntry*)((*iter)->reserved);
				if (entry->m_initialized)
				{
					if ((*iter)->close_func)
					{
						(*iter)->close_func();
					}
					entry->m_initialized = false;
				}
			}
			init_queue.clear();
			g_module_init_queue.destruct();
		}

		return r;
	}

	void module_close()
	{
		auto init_queue = g_module_init_queue.get();
		for (auto iter = init_queue.rbegin(); iter != init_queue.rend(); ++iter)
		{
			ModuleEntry* entry = (ModuleEntry*)((*iter)->reserved);
			if (entry->m_initialized)
			{
				if ((*iter)->close_func)
				{
					(*iter)->close_func();
				}
				entry->m_initialized = false;
			}
		}
		init_queue.clear();
		g_module_init_queue.destruct();
	}

	LUNA_RUNTIME_API RV init_modules()
	{
		// Fill modules.
		HashMap<Name, ModuleDesc*> modules;
		{
			ModuleDesc* iter = g_module_header;
			while (iter)
			{
				modules.insert(Pair<Name, ModuleDesc*>(Name(iter->name), iter));
				ModuleEntry* entry = (ModuleEntry*)(iter->reserved);
				iter = entry->m_next;
			}
		}
		HashMap<Name, ModuleDesc*> unresolved_modules = modules;
		{
			// Exclude all resolved or initialized modules.
			auto iter = unresolved_modules.begin();
			while (iter != unresolved_modules.end())
			{
				ModuleEntry* entry = (ModuleEntry*)(iter->second->reserved);
				if (entry->m_resolved || entry->m_initialized)
				{
					iter = unresolved_modules.erase(iter);
				}
				else
				{
					++iter;
				}
			}
		}
		// Rebuild init queue.
		auto& init_queue = g_module_init_queue.get();
		
		// Loop until all modules are resolved and pushed into the queue.
		while (!unresolved_modules.empty())
		{
			bool any_removed = false;
			auto iter = unresolved_modules.begin();
			while (iter != unresolved_modules.end())
			{
				bool can_init = true;	// If this module can be initialized.

				// Check dependencies.
				const c8* cur = (*iter).second->dependencies;
				while (cur && *cur != '\0')
				{
					// Find next ';'
					const c8* end = strchr(cur, ';');
					Name dep;
					if (end)
					{
						dep = Name(cur, end - cur);
					}
					else
					{
						dep = Name(cur);
					}
					auto dep_iter = modules.find(dep);

					if (dep_iter == modules.end())
					{
						//get_error().set(e_not_found, "Module %s required by module %s is not found.", iter->second->name, dep.c_str());
						return BasicError::not_found();
					}
					ModuleEntry* dep_entry = (ModuleEntry*)(dep_iter->second->reserved);
					if (!dep_entry->m_resolved)
					{
						// The dependency is not initialized.
						can_init = false;
						break;
					}
					if (end)
					{
						cur = end + 1;
					}
					else
					{
						break;
					}
				}
				if (can_init)
				{
					// Remove this.
					init_queue.push_back(iter->second);
					ModuleEntry* entry = (ModuleEntry*)(iter->second->reserved);
					entry->m_resolved = true;
					any_removed = true;
					iter = unresolved_modules.erase(iter);
				}
				else
				{
					++iter; // Skip this, leave to next roll.
				}
			}
			if (!any_removed)
			{
				// Cycle reference detected.
				//get_error().set(e_bad_arguments, "Cycling module dependencies detected.");
				return BasicError::bad_arguments();
			}
		}

		// Initialize all uninitialized modules.
		RV r;
		for (auto i : init_queue)
		{
			ModuleEntry* entry = (ModuleEntry*)(i->reserved);
			luassert(entry->m_resolved);
			if (!entry->m_initialized)
			{
				if (i->init_func)
				{
					r = i->init_func();
					if (!r.valid())
					{
						return r;
					}
				}
				entry->m_initialized = true;
			}
		}
		return r;
	}
}
