// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file AssetSystem.cpp
* @author JXMaster
* @date 2020/2/15
*/
#include "AssetSystem.hpp"
#include "AssetMeta.hpp"
#include <Runtime/Module.hpp>
namespace Luna
{
	namespace Asset
	{
		Name g_name_type;
		Name g_name_guid;
		Name g_name_data_path;
		Name g_name_attachments;
		Name g_name_dependencies;
		Unconstructed<HashMap<Name, P<IAssetType>>> g_types;
		P<IMutex> g_type_lock;
		Unconstructed<HashMap<Guid, P<IAsset>>> g_assets;
		Unconstructed<HashMap<Path, Guid>> g_path_mapping;
		P<IMutex> g_lock;
		P<IDispatchQueue> g_dispatch;
		Unconstructed<HashMap<Guid, Vector<Guid>>> m_unresolved;

		RV init()
		{
			g_assets.construct();
			g_types.construct();
			g_path_mapping.construct();
			m_unresolved.construct();
			g_lock = new_mutex();
			g_type_lock = new_mutex();
			g_dispatch = new_dispatch_queue(1);
			g_name_type = u8"type";
			g_name_attachments = u8"attachments";
			g_name_dependencies = u8"dependencies";
			g_name_guid = u8"guid";
			g_name_data_path = u8"data_path";
			return RV();
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

		StaticRegisterModule m("Asset", "Core", init, deinit);

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
					Pair<Guid, Vector<Guid>> p(move(to), move(Vector<Guid>()));
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

		R<Variant> load_asset_from_file(const Path& path, bool load_meta)
		{
			Variant r;
			lutry
			{
				auto p = path;
				P<IFile> f;
				bool is_binary = false;
				if (load_meta)
				{
					const c8 ext[] = ".meta.la";
					auto filename = p[p.size() - 1];
					auto filename_sz = strlen(filename.c_str());
					c8* filename_ext = (c8*)alloca(sizeof(c8) * (filename_sz + 9));
					memcpy(filename_ext, filename.c_str(), filename_sz * sizeof(c8));
					memcpy(filename_ext + filename_sz, ext, 9 * sizeof(c8));
					auto filename_name = Name(filename_ext);
					p[p.size() - 1] = filename_name;
					auto rf = open_file(p, EFileOpenFlag::read | EFileOpenFlag::user_buffering, EFileCreationMode::open_existing);
					if (succeeded(rf))
					{
						f = rf.get();
					}
					else
					{
						is_binary = true;
						const c8 ext2[] = ".meta.lb";
						memcpy(filename_ext, filename.c_str(), filename_sz * sizeof(c8));
						memcpy(filename_ext + filename_sz, ext2, 9 * sizeof(c8));
						filename_name = Name(filename_ext);
						p[p.size() - 1] = filename_name;
						rf = open_file(p, EFileOpenFlag::read | EFileOpenFlag::user_buffering, EFileCreationMode::open_existing);
						if (failed(rf))
						{
							return rf.errcode();
						}
						f = rf.get();
					}
				}
				else
				{
					const c8 ext[] = ".data.la";
					auto filename = p[p.size() - 1];
					auto filename_sz = strlen(filename.c_str());
					char* filename_ext = (c8*)alloca(sizeof(c8) * (filename_sz + 9));
					memcpy(filename_ext, filename.c_str(), filename_sz * sizeof(c8));
					memcpy(filename_ext + filename_sz, ext, 9 * sizeof(c8));
					auto filename_name = Name(filename_ext);
					p[p.size() - 1] = filename_name;
					auto rf = open_file(p, EFileOpenFlag::read | EFileOpenFlag::user_buffering, EFileCreationMode::open_existing);
					if (succeeded(rf))
					{
						f = rf.get();
					}
					else
					{
						is_binary = true;
						const c8 ext2[] = ".data.lb";
						memcpy(filename_ext, filename.c_str(), filename_sz * sizeof(c8));
						memcpy(filename_ext + filename_sz, ext2, 9 * sizeof(c8));
						filename_name = Name(filename_ext);
						p[p.size() - 1] = filename_name;
						rf = open_file(p, EFileOpenFlag::read | EFileOpenFlag::user_buffering, EFileCreationMode::open_existing);
						if (failed(rf))
						{
							return rf.errcode();
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
			if (!(ass->meta()->meta_path().empty()))
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
			lucheck(type_obj);
			MutexGuard g(g_type_lock);
			auto name = type_obj->type_name();
			auto iter = g_types.get().find(name);
			if (iter != g_types.get().end())
			{
				return BasicError::already_exists();
			}
			g_types.get().insert(Pair<Name, WP<IAssetType>>(name, type_obj));
			return RV();
		}

		RV unregister_asset_type(const Name& type_name)
		{
			lucheck(type_name);
			MutexGuard g(g_type_lock);
			auto iter = g_types.get().find(type_name);
			if (iter == g_types.get().end())
			{
				return BasicError::not_found();
			}
			g_types.get().erase(iter);
			return RV();
		}

		RP<IAssetType> get_asset_type(const Name& type_name)
		{
			lucheck(type_name);
			MutexGuard g(g_type_lock);
			auto iter = g_types.get().find(type_name);
			if (iter == g_types.get().end())
			{
				return BasicError::not_found();
			}
			return iter->second;
		}

		RP<IAsset> load_asset_meta(const Path& meta_path)
		{
			P<IAsset> ass;
			lutry
			{
				MutexGuard g(g_lock);

				lulet(meta_var, load_asset_from_file(meta_path, true));
				// Parse data object to fetch meta information.
				Guid guid;
				auto guid_field = meta_var.field(0, g_name_guid);
				lulet(u64_buf, guid_field.check_u64_buf());
				guid.low = u64_buf[0];
				guid.high = u64_buf[1];
				auto test_ass = fetch_asset(guid);
				if (succeeded(test_ass))
				{
					return test_ass.get();
				}

				P<AssetMeta> meta = newobj<AssetMeta>();
				
				meta->m_guid = guid;
				meta->m_meta_path = meta_path;

				// Load type.
				lulet(type_name, meta_var.field(0, g_name_type).check_name());
				meta->m_type = type_name;

				// Load data path.
				auto data_path_field = meta_var.field(0, g_name_data_path);
				if (data_path_field.type() == EVariantType::path)
				{
					auto& data_path = data_path_field.to_path();
					if ((data_path.flags() & EPathFlag::absolute) == EPathFlag::none)
					{
						auto abs_data_path = meta->m_meta_path;
						abs_data_path.append(data_path);
						meta->m_data_path = abs_data_path;
					}
					else
					{
						meta->m_data_path = data_path;
					}
				}
				else
				{
					// Same as meta path.
					meta->m_data_path = meta->m_meta_path;
				}
				
				// Load dependencies.
				auto& dependencies_field = meta_var.field(0, g_name_dependencies);
				if (dependencies_field.type() != EVariantType::null)
				{
					usize num_dependencies = dependencies_field.length(2);
					lulet(dependencies_buf, dependencies_field.check_u64_buf());
					for (usize i = 0; i < num_dependencies; ++i)
					{
						Guid guid2;
						guid2.low = dependencies_buf[dependencies_field.index(0, i)];
						guid2.high = dependencies_buf[dependencies_field.index(1, i)];
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

		RP<IAsset> new_asset(const Name& asset_type, const Guid* guid)
		{
			lucheck(asset_type);
			P<IAsset> ass;
			lutry
			{
				if (guid)
				{
					auto fr = fetch_asset(*guid);
					if (succeeded(fr))
					{
						return fr.get();
					}
				}
				MutexGuard g(g_lock);
				P<AssetMeta> meta = newobj<AssetMeta>();
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
				return BasicError::not_found();
			}
			return ass;
		}

		RP<IAsset> fetch_asset(const Path& meta_path)
		{
			MutexGuard g(g_lock);
			auto iter = g_path_mapping.get().find(meta_path);
			if (iter != g_path_mapping.get().end())
			{
				return fetch_asset(iter->second);
			}
			else
			{
				return BasicError::not_found();
			}
		}

		RV remove_asset(const Guid& asset_id)
		{
			MutexGuard g(g_lock);

			auto iter = g_assets.get().find(asset_id);
			if (iter == g_assets.get().end())
			{
				return BasicError::not_found();
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
			return RV();
		}
	}
}