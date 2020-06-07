// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file AssetMeta.cpp
* @author JXMaster
* @date 2020/4/17
*/
#include "AssetMeta.hpp"
#include "AssetSystem.hpp"
#include "AssetRequests.hpp"

namespace luna
{
	namespace asset
	{
		void AssetMeta::remove_dependent(const Guid& guid)
		{
			auto i = m_dependents.begin();
			while (i != m_dependents.end())
			{
				if (*i == guid)
				{
					m_dependents.erase(i);
					return;
				}
			}
		}

		void AssetMeta::set_data_path(IPath* path)
		{
			luassert_usr(m_valid);
			MutexGuard g(m_mtx);
			if (path)
			{
				luassert_msg_usr((path->flags() & EPathFlag::absolute) != EPathFlag::none, "IAssetMeta::set_data_path - The data and meta path for one asset must be absolute");
				auto npath = new_path();
				npath->assign(path);
				m_data_path = npath;
			}
			else
			{
				m_data_path = nullptr;
			}
		}

		RV AssetMeta::set_meta_path(IPath* path)
		{
			luassert_usr(m_valid);
			MutexGuard g(g_lock);
			MutexGuard g2(m_mtx);

			// Check collision.
			if (path)
			{
				luassert_msg_usr((path->flags() & EPathFlag::absolute) != EPathFlag::none, "IAssetMeta::set_meta_path - The data and meta path for one asset must be absolute");
				auto iter = g_path_mapping.get().find(path);
				if (iter != g_path_mapping.get().end())
				{
					set_err(e_item_already_exists, "The path %s is already used by another asset.", path->encode()->c_str());
					return e_user_failure;
				}
			}

			// Remove old record.
			if (m_meta_path)
			{
				auto eiter = g_path_mapping.get().find(m_meta_path);
				if (eiter != g_path_mapping.get().end())
				{
					g_path_mapping.get().erase(eiter);
				}
			}
			
			// Insert new record.
			if (path)
			{
				auto npath = new_path();
				npath->assign(path);
				m_meta_path = npath;
				// Add new record.
				g_path_mapping.get().insert(Pair<P<IPath>, Guid>(npath, m_guid));
			}
			else
			{
				m_meta_path = nullptr;
			}
			return s_ok;
		}

		bool AssetMeta::unused()
		{
			luassert_usr(m_valid);
			MutexGuard g(m_mtx);
			if (m_pin_count)
			{
				return false;
			}
			for (auto& i : m_dependents)
			{
				auto ass = fetch_asset(i);
				if (failed(ass))
				{
					continue;
				}
				auto meta = ass.get()->meta();
				if (meta->state() != EAssetState::unloaded)
				{
					return false;
				}
			}
			return true;
		}

		void AssetMeta::load(EAssetLoadFlag flags, IVariant* params)
		{
			luassert_usr(m_valid);
			MutexGuard g(m_mtx);
			// Early out.
			if (m_state == EAssetState::loading)
			{
				return;
			}
			if (m_state == EAssetState::loaded && ((flags & EAssetLoadFlag::force_reload) == EAssetLoadFlag::none))
			{
				return;
			}

			// Load dependencies first.
			for (auto& i : m_dependencies)
			{
				auto ass = fetch_asset(i);
				if (succeeded(ass))
				{
					IAssetMeta* ass_meta = ass.get()->meta();
					MutexGuard ass_lock(ass_meta->mutex());
					if (ass_meta->state() == EAssetState::unloaded)
					{
						ass_meta->load(EAssetLoadFlag::none);
					}
				}
			}

			// Clears the error flag.
			m_flags = m_flags & ~EAssetFlag::loading_error;

			// Create a new load request.
			m_state = EAssetState::loading;

			P<AssetLoadRequest> request = box_ptr(new_obj<AssetLoadRequest>(get_module_allocator()));
			request->m_asset = m_asset;
			request->m_params = params;
			request->m_flags = flags;
			g_dispatch->dispatch(request);
		}

		void AssetMeta::unload(EAssetUnloadFlag flags)
		{
			luassert_usr(m_valid);
			MutexGuard g(m_mtx);
			if (m_state != EAssetState::loaded)
			{
				return;
			}

			auto cur_ass = m_asset.lock();

			// Dispatch unload message.
			for (auto& i : m_dependents)
			{
				auto ass = fetch_asset(i);
				if (failed(ass))
				{
					continue;
				}
				AssetMeta* meta = static_cast<AssetMeta*>(ass.get()->meta());
				auto dep_mgr = meta->m_type_obj;
				MutexGuard dep_guard(meta->mutex());
				if (meta->state() == EAssetState::loaded)
				{
					dep_mgr->on_dependency_data_unload(ass.get(), cur_ass);
				}
			}

			m_type_obj->on_unload_data(cur_ass);
			m_state = EAssetState::unloaded;
			// Check & unload dependency.
			if ((flags & EAssetUnloadFlag::no_unload_unused_deps) == EAssetUnloadFlag::none)
			{
				for (auto& i : m_dependencies)
				{
					auto ass = fetch_asset(i);
					if (failed(ass))
					{
						continue;
					}
					AssetMeta* meta = static_cast<AssetMeta*>(ass.get()->meta());
					if (meta->unused() && (meta->state() == EAssetState::loaded))
					{
						meta->unload();
					}
				}
			}
		}

		RP<IAssetSaveRequest> AssetMeta::save_data(EAssetSaveFormat save_format, IVariant* params)
		{
			luassert_usr(m_valid);
			P<AssetSaveRequest> req;
			MutexGuard g(m_mtx);
			if (m_state != EAssetState::loaded)
			{
				set_err(e_bad_calling_time, "IAssetMeta::save_data - Cannot save data when the data is not loaded.");
				return e_user_failure;
			}
			req = box_ptr(new_obj<AssetSaveRequest>(get_module_allocator()));
			req->m_asset = m_asset;
			req->m_save_data = true;
			req->m_format = save_format;
			req->m_params = params;
			g_dispatch->dispatch(req);
			return req;
		}

		RP<IAssetSaveRequest> AssetMeta::save_meta(EAssetSaveFormat save_format)
		{
			luassert_usr(m_valid);
			P<AssetSaveRequest> req;
			MutexGuard g(m_mtx);
			req = box_ptr(new_obj<AssetSaveRequest>(get_module_allocator()));
			req->m_asset = m_asset;
			req->m_save_data = false;
			req->m_format = save_format;
			g_dispatch->dispatch(req);
			return req;
		}

		void AssetMeta::internal_add_dependency(const Guid& guid)
		{
			luassert_usr(m_valid);
			MutexGuard g(m_mtx);
			if (guid == Guid(0, 0))
			{
				return;
			}
			m_dependencies.push_back(guid);
			asset::add_dependency(this, guid);
		}

		bool AssetMeta::internal_remove_dependency(const Guid& guid)
		{
			luassert_usr(m_valid);
			MutexGuard g(m_mtx);
			if (guid == Guid(0, 0))
			{
				return false;
			}
			if (!asset::remove_dependency(this, guid))
			{
				return false;
			}
			for (auto iter = m_dependencies.begin(); iter != m_dependencies.end(); ++iter)
			{
				if (*iter == guid)
				{
					m_dependencies.erase(iter);
					break;
				}
			}
			return true;
		}

		RV AssetMeta::replace_dependency(const Guid& before, const Guid& after)
		{
			luassert_usr(m_valid);
			MutexGuard g(m_mtx);
			if (m_state != EAssetState::loaded)
			{
				return e_bad_calling_time;
			}
			m_type_obj->on_dependency_replace(m_asset.lock(), before, after);
			// Removes all old dependency, and adds new dependency.
			auto deps = m_dependencies;
			for (auto& i : deps)
			{
				if (i == before)
				{
					auto _ = internal_remove_dependency(before);
					internal_add_dependency(after);
				}
			}
			return s_ok;
		}
		void AssetMeta::internal_remove_all_dependencies()
		{
			luassert_usr(m_valid);
			MutexGuard g(m_mtx);
			for (auto& i : m_dependencies)
			{
				asset::remove_dependency(this, i);
			}
			m_dependencies.clear();
		}
	}
}