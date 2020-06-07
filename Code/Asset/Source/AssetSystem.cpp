// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file AssetSystem.cpp
* @author JXMaster
* @date 2020/2/15
*/
#include "AssetSystem.hpp"
#include "AssetMeta.hpp"
namespace luna
{
	namespace asset
	{
		P<IName> g_name_type;
		P<IName> g_name_guid;
		P<IName> g_name_data_path;
		P<IName> g_name_attachments;
		P<IName> g_name_dependencies;
		Unconstructed<HashMap<P<IName>, P<IAssetType>>> g_types;
		P<IReadWriteLock> g_type_lock;
		Unconstructed<HashMap<Guid, P<IAsset>>> g_assets;
		Unconstructed<HashMap<P<IPath>, Guid>> g_path_mapping;
		P<IMutex> g_lock;
		P<IDispatchQueue> g_dispatch;
		Unconstructed<HashMap<Guid, Vector<Guid>>> m_unresolved;

		LUNA_EXPORT void init()
		{
			g_assets.construct(get_module_allocator());
			g_types.construct(get_module_allocator());
			g_path_mapping.construct(get_module_allocator());
			m_unresolved.construct(get_module_allocator());
			g_lock = new_mutex();
			g_type_lock = new_read_write_lock();
			g_dispatch = new_dispatch_queue(1);
			g_name_type = intern_name("type");
			g_name_attachments = intern_name("attachments");
			g_name_dependencies = intern_name("dependencies");
			g_name_guid = intern_name("guid");
			g_name_data_path = intern_name("data_path");
			add_module("Asset", deinit);
		}

		void deinit()
		{
			g_name_data_path = nullptr;
			g_name_guid = nullptr;
			g_name_dependencies = nullptr;
			g_name_attachments = nullptr;
			g_name_type = nullptr;
			g_dispatch = nullptr;
			g_type_lock = nullptr;
			g_lock = nullptr;
			m_unresolved.destruct();
			g_path_mapping.destruct();
			g_types.destruct();
			g_assets.destruct();
		}

		void add_dependency(AssetMeta* from, const Guid& to)
		{
			MutexGuard g(g_lock);
			auto ass = fetch_asset(to);
			if (failed(ass))
			{
				// Adds unresolved.
				auto iter = m_unresolved.get().find(to);
				if (iter == m_unresolved.get().end())
				{
					Pair<Guid, Vector<Guid>> p(move(to), move(Vector<Guid>(get_module_allocator())));
					iter = m_unresolved.get().insert(move(p)).first;
				}
				iter->second.push_back(from->guid());
			}
			else
			{
				AssetMeta* meta = static_cast<AssetMeta*>(ass.get()->meta());
				meta->m_dependents.push_back(from->guid());
			}
		}

		bool remove_dependency(AssetMeta* from, const Guid& to)
		{
			MutexGuard g(g_lock);
			auto ass = fetch_asset(to);
			if (failed(ass))
			{
				// Remove unresolved.
				auto iter = m_unresolved.get().find(to);
				if (iter != m_unresolved.get().end())
				{
					auto& vec = iter->second;
					for (auto iter2 = vec.begin(); iter2 != vec.end(); ++iter2)
					{
						if (*iter2 == from->guid())
						{
							vec.erase(iter2);
							if (vec.empty())
							{
								m_unresolved.get().erase(iter);
							}
							return true;
						}
					}
				}
				// Asset not found.
				return false;
			}
			else
			{
				AssetMeta* meta = static_cast<AssetMeta*>(ass.get()->meta());
				for (auto iter = meta->m_dependents.begin(); iter != meta->m_dependents.end(); ++iter)
				{
					if (*iter == from->guid())
					{
						meta->m_dependents.erase(iter);
						return true;
					}
				}
				// Asset not found.
				return false;
			}
			return true;
		}

		RP<IVariant> load_asset_from_file(IPath* path, bool load_meta)
		{
			P<IVariant> r;
			lutry
			{
				auto p = new_path();
				p->assign(path);
				P<IFile> f;
				bool is_binary = false;
				if (load_meta)
				{
					const char ext[] = ".meta.la";
					auto filename = p->node(p->count_nodes() - 1);
					char* filename_ext = (char*)alloca(sizeof(char) * (filename->size() + 9));
					memcpy(filename_ext, filename->c_str(), filename->size() * sizeof(char));
					memcpy(filename_ext + filename->size(), ext, 9 * sizeof(char));
					auto filename_name = intern_name(filename_ext);
					p->set_node(p->count_nodes() - 1, filename_name);
					auto rf = open_file(p, EFileOpenFlag::read | EFileOpenFlag::user_buffering, EFileCreationMode::open_existing);
					if (succeeded(rf))
					{
						f = rf.get();
					}
					else
					{
						is_binary = true;
						const char ext2[] = ".meta.lb";
						memcpy(filename_ext, filename->c_str(), filename->size() * sizeof(char));
						memcpy(filename_ext + filename->size(), ext2, 9 * sizeof(char));
						filename_name = intern_name(filename_ext);
						p->set_node(p->count_nodes() - 1, filename_name);
						rf = open_file(p, EFileOpenFlag::read | EFileOpenFlag::user_buffering, EFileCreationMode::open_existing);
						if (failed(rf))
						{
							return rf.result();
						}
						f = rf.get();
					}
				}
				else
				{
					const char ext[] = ".data.la";
					auto filename = p->node(p->count_nodes() - 1);
					char* filename_ext = (char*)alloca(sizeof(char) * (filename->size() + 9));
					memcpy(filename_ext, filename->c_str(), filename->size() * sizeof(char));
					memcpy(filename_ext + filename->size(), ext, 9 * sizeof(char));
					auto filename_name = intern_name(filename_ext);
					p->set_node(p->count_nodes() - 1, filename_name);
					auto rf = open_file(p, EFileOpenFlag::read | EFileOpenFlag::user_buffering, EFileCreationMode::open_existing);
					if (succeeded(rf))
					{
						f = rf.get();
					}
					else
					{
						is_binary = true;
						const char ext2[] = ".data.lb";
						memcpy(filename_ext, filename->c_str(), filename->size() * sizeof(char));
						memcpy(filename_ext + filename->size(), ext2, 9 * sizeof(char));
						filename_name = intern_name(filename_ext);
						p->set_node(p->count_nodes() - 1, filename_name);
						rf = open_file(p, EFileOpenFlag::read | EFileOpenFlag::user_buffering, EFileCreationMode::open_existing);
						if (failed(rf))
						{
							return rf.result();
						}
						f = rf.get();
					}
				}
				P<IDecoder> decoder;
				if (is_binary)
				{
					lupanic_msg("Not implemented.");
				}
				else
				{
					decoder = new_text_decoder();
				}
				luset(r, decoder->decode(f));
			}
			lucatchret;
			return r;
		}

		void insert_asset(IAsset* ass, AssetMeta* meta)
		{
			// Inserts the asset to the registry.
			g_assets.get().insert(Pair<Guid, P<IAsset>>(meta->m_guid, ass));

			// Inserts the path to the registry if not nullptr.
			if (ass->meta()->meta_path())
			{
				g_path_mapping.get().insert_or_assign(ass->meta()->meta_path(), ass->meta()->guid());
			}

			// Checks unresolved and removes them if any.
			auto uiter = m_unresolved.get().find(meta->m_guid);
			if (uiter != m_unresolved.get().end())
			{
				meta->m_dependents = move(uiter->second);
				m_unresolved.get().erase(uiter);
			}
		}

		RV register_asset_type(IAssetType* type_obj)
		{
			luassert_usr(type_obj);
			WriteGuard g(g_type_lock);
			auto name = type_obj->type_name();
			auto iter = g_types.get().find(name);
			if (iter != g_types.get().end())
			{
				return e_item_already_exists;
			}
			g_types.get().insert(Pair<P<IName>, WP<IAssetType>>(name, type_obj));
			return s_ok;
		}

		RV unregister_asset_type(IName* type_name)
		{
			luassert_usr(type_name);
			WriteGuard g(g_type_lock);
			auto iter = g_types.get().find(type_name);
			if (iter == g_types.get().end())
			{
				return e_item_not_exist;
			}
			g_types.get().erase(iter);
			return s_ok;
		}

		RP<IAssetType> get_asset_type(IName* type_name)
		{
			luassert_usr(type_name);
			WriteGuard g(g_type_lock);
			auto iter = g_types.get().find(type_name);
			if (iter == g_types.get().end())
			{
				return e_item_not_exist;
			}
			return iter->second;
		}

		RP<IAsset> load_asset_meta(IPath* meta_path)
		{
			luassert_usr(meta_path);
			P<IAsset> ass;
			lutry
			{
				MutexGuard g(g_lock);

				lulet(meta_var, load_asset_from_file(meta_path, true));
				// Parse data object to fetch meta information.
				Guid guid;
				lulet(guid_field, meta_var->field(0, g_name_guid));
				lulet(u64_buf, guid_field->check_u64_buf());
				guid.low = u64_buf[0];
				guid.high = u64_buf[1];
				auto test_ass = fetch_asset(guid);
				if (succeeded(test_ass))
				{
					return RP<IAsset>(test_ass.get(), s_already_done);
				}

				P<AssetMeta> meta = box_ptr(new_obj<AssetMeta>(get_module_allocator()));
				if (!meta)
				{
					return e_bad_memory_alloc;
				}
				
				meta->m_guid = guid;
				meta->m_meta_path = meta_path->clone();
				meta->m_error = new_err();

				// Load type.
				lulet(type_field, meta_var->field(0, g_name_type));
				lulet(type_name, type_field->check_name());
				meta->m_type = type_name;

				// Load data path.
				auto data_path_field = meta_var->field(0, g_name_data_path);
				if (succeeded(data_path_field) && (data_path_field.get()->type() == EVariantType::path))
				{
					auto data_path = data_path_field.get()->path();
					if ((data_path->flags() & EPathFlag::absolute) == EPathFlag::none)
					{
						auto abs_data_path = new_path();
						abs_data_path->assign(meta->m_meta_path);
						abs_data_path->append(data_path);
						meta->m_data_path = abs_data_path;
					}
					else
					{
						meta->m_data_path = data_path->clone();
					}
				}
				else
				{
					// Same as meta path.
					meta->m_data_path = meta->m_meta_path->clone();
				}
				
				// Load dependencies.
				auto dependencies_field = meta_var->field(0, g_name_dependencies);
				if (succeeded(dependencies_field))
				{
					size_t num_dependencies = dependencies_field.get()->length(2);
					lulet(dependencies_buf, dependencies_field.get()->check_u64_buf());
					for (size_t i = 0; i < num_dependencies; ++i)
					{
						Guid guid2;
						guid2.low = dependencies_buf[dependencies_field.get()->index(0, i)];
						guid2.high = dependencies_buf[dependencies_field.get()->index(1, i)];
						meta->internal_add_dependency(guid2);
					}
				}

				// Create asset object.
				lulet(mgr, route_mgr(meta->m_type));
				meta->m_type_obj = mgr;
				ass = mgr->on_new_asset(meta);
				insert_asset(ass, meta);
				meta->m_asset = ass;

				// Notify dependents.
				//for (auto& i : meta->m_dependents)
				//{
				//	auto depass = fetch_asset(i);
				//	if (failed(depass))
				//	{
				//		continue;
				//	}
				//	auto mgr = static_cast<AssetMeta*>(depass.get()->meta())->m_type_obj;
				//	MutexGuard depg(depass.get()->meta()->mutex());
				//	mgr->on_dependency_create(depass.get(), ass);
				//}
			}
			lucatchret;
			return ass;
		}

		RP<IAsset> new_asset(IName* asset_type, const Guid* guid)
		{
			luassert_usr(asset_type);
			P<IAsset> ass;
			lutry
			{
				if (guid)
				{
					auto fr = fetch_asset(*guid);
					if (succeeded(fr))
					{
						return RP<IAsset>::success(fr.get(), s_already_done);
					}
				}
				MutexGuard g(g_lock);
				P<AssetMeta> meta = box_ptr(new_obj<AssetMeta>(get_module_allocator()));
				if (!meta)
				{
					return e_bad_memory_alloc;
				}
				if (guid)
				{
					meta->m_guid = *guid;
				}
				else
				{
					meta->m_guid = random_guid();
				}
				meta->m_type = asset_type;
				lulet(mgr, route_mgr(meta->m_type));
				meta->m_type_obj = mgr;
				// Create asset object.
				ass = mgr->on_new_asset(meta);
				// Inserts the asset to the registry.
				insert_asset(ass, meta);
				meta->m_asset = ass;

				// Notify dependents.
				//for (auto& i : meta->m_dependents)
				//{
				//	auto depass = fetch_asset(i);
				//	if (failed(depass))
				//	{
				//		continue;
				//	}
				//	auto mgr = static_cast<AssetMeta*>(depass.get()->meta())->m_type_obj;
				//	MutexGuard depg(depass.get()->meta()->mutex());
				//	mgr->on_dependency_create(depass.get(), ass);
				//}
			}
			lucatchret;
			return ass;
		}

		RP<IAsset> fetch_asset(const Guid& asset_id)
		{
			P<IAsset> ass;
			MutexGuard g(g_lock);

			auto iter = g_assets.get().find(asset_id);
			if (iter != g_assets.get().end())
			{
				ass = iter->second;
			}
			else
			{
				return e_item_not_exist;
			}
			return ass;
		}

		RP<IAsset> fetch_asset(IPath* meta_path)
		{
			MutexGuard g(g_lock);
			auto iter = g_path_mapping.get().find(meta_path);
			if (iter != g_path_mapping.get().end())
			{
				return fetch_asset(iter->second);
			}
			else
			{
				return e_item_not_exist;
			}
		}

		RV remove_asset(const Guid& asset_id)
		{
			MutexGuard g(g_lock);

			auto iter = g_assets.get().find(asset_id);
			if (iter == g_assets.get().end())
			{
				return e_item_not_exist;
			}
			P<IAsset> ass = iter->second;
			
			AssetMeta* meta = static_cast<AssetMeta*>(ass->meta());
			MutexGuard meta_guard(meta->m_mtx);
			// Unload the asset first if it is loaded.
			meta->unload();
			// Check and notify all dependents.
			auto dependents = meta->dependents();
			//for (auto& i : dependents)
			//{
			//	auto ass = fetch_asset(i);
			//	if (failed(ass))
			//	{
			//		continue;
			//	}
			//	auto mgr = static_cast<AssetMeta*>(ass.get()->meta())->m_type_obj;
			//	MutexGuard depg(ass.get()->meta()->mutex());
			//	mgr->on_dependency_remove(ass.get(), iter->second);
			//}
			// If there are still dependents, add them to unresolved.
			if (!meta->m_dependents.empty())
			{
				m_unresolved.get().insert(Pair<Guid, Vector<Guid>>(meta->m_guid, move(meta->m_dependents)));
			}
			// Remove all dependencies.
			while (!meta->m_dependencies.empty())
			{
				auto _ = meta->internal_remove_dependency(meta->m_dependencies.front());
			}
#ifdef LUNA_PROFILE
			meta->m_valid = false;
#endif
			meta->m_type_obj = nullptr;
			g_assets.get().erase(iter);
			auto piter = g_path_mapping.get().find(meta->m_meta_path);
			if (piter != g_path_mapping.get().end())
			{
				g_path_mapping.get().erase(piter);
			}
			return s_ok;
		}
	}
}