// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file AssetRequest.cpp
* @author JXMaster
* @date 2020/4/18
*/
#include "AssetRequests.hpp"
#include "AssetSystem.hpp"
#include "AssetMeta.hpp"

namespace Luna
{
	namespace Asset
	{
		void AssetLoadRequest::AssetLoadRequest::run()
		{
			auto ass = m_asset.lock();
			if (!ass)
			{
				return;
			}
			auto meta = static_cast<AssetMeta*>(ass->meta());
			MutexGuard g(meta->mutex());
			lutry
			{
				lulet(mgr, route_mgr(meta->type()));
				// Load data file.
				if ((m_flags & EAssetLoadFlag::procedural) == EAssetLoadFlag::none)
				{
					lulet(var, load_asset_from_file(meta->data_path(), false));
					luexp(mgr->on_load_data(ass, var, m_params));
				}
				else
				{
					luexp(mgr->on_load_procedural_data(ass, m_params));
				}
				meta->internal_set_state(EAssetState::loaded);
				// Dispatch load event.
				for (auto& i : meta->m_dependents)
				{
					auto dep_ass = fetch_asset(i);
					if (failed(dep_ass))
					{
						continue;
					}
					AssetMeta* meta = static_cast<AssetMeta*>(dep_ass.get()->meta());
					auto dep_mgr = route_mgr(meta->m_type);
					if (failed(dep_mgr))
					{
						continue;
					}
					MutexGuard dep_guard(meta->mutex());
					if (meta->state() == EAssetState::loaded)
					{
						dep_mgr.get()->on_dependency_data_load(dep_ass.get(), ass);
					}
				}
			}
			lucatch
			{
				meta->internal_set_state(EAssetState::unloaded);
				meta->internal_set_flags(meta->flags() | EAssetFlag::loading_error);
				// Failed to load data.
				if (lures == BasicError::error_object())
				{
					meta->error_object() = get_error_object();
				}
				else
				{
					meta->error_object() = Error(lures);
				}
			}
			return;
		}

		void AssetSaveRequest::run()
		{
			auto ass = m_asset.lock();
			if (!ass)
			{
				return;
			}
			auto meta = ass->meta();
			MutexGuard g(meta->mutex());
			lutry
			{
				if (m_save_data)
				{
					// Save data.
					lulet(mgr, route_mgr(meta->type()));
					lulet(data, mgr->on_save_data(ass, m_params));
					auto save_path = meta->data_path();
					
					if (m_format == EAssetSaveFormat::ascii)
					{
						save_path.append_extension("data.la");
					}
					else
					{
						lupanic();
					}
					lulet(f, open_file(save_path, EFileOpenFlag::write | EFileOpenFlag::user_buffering, EFileCreationMode::create_always));
					auto encoder = new_text_encoder();
					luexp(encoder->encode(data, f));
					f = nullptr;
				}
				else
				{
					// Save meta.
					// Records in meta:
					// * "type" - The type of the asset.
					// * "data_path" - Optional. If the path of the data is not the path of the asset, stores the 
					// path of the data relative to the path of the asset.
					// * "dependencies" - Array of Guids of all assets this asset depends on.
					auto var = Variant(EVariantType::table);
					// Save type.
					auto var_type = Variant(EVariantType::name);
					var_type.to_name() = meta->type();
					var.set_field(0, g_name_type, var_type);
					// Save guid.
					auto var_guid = Variant(EVariantType::u64, 2);
					Guid guid = meta->guid();
					var_guid.to_u64_buf()[0] = guid.low;
					var_guid.to_u64_buf()[1] = guid.high;
					var.set_field(0, g_name_guid, var_guid);
					// Save data path only if not default.
					if (!meta->data_path().equal_to(meta->meta_path()))
					{
						auto relative_path = Path();
						relative_path.assign_relative(meta->meta_path(), meta->data_path());
						auto path_var = Variant(EVariantType::path);
						path_var.to_path() = relative_path;
						var.set_field(0, g_name_data_path, path_var);
					}
					// Save dependencies if not empty.
					auto deps = meta->dependencies();
					if (!deps.empty())
					{
						auto deps_var = Variant(EVariantType::u64, 2, deps.size());
						for (usize i = 0; i < deps.size(); ++i)
						{
							deps_var.to_u64_buf()[deps_var.index(0, i)] = deps[i].low;
							deps_var.to_u64_buf()[deps_var.index(1, i)] = deps[i].high;
						}
						var.set_field(0, g_name_dependencies, deps_var);
					}
					// Write to file.
					auto save_path = meta->meta_path();

					if (m_format == EAssetSaveFormat::ascii)
					{
						save_path.append_extension("meta.la");
					}
					else
					{
						lupanic();
					}
					lulet(f, open_file(save_path, EFileOpenFlag::write | EFileOpenFlag::user_buffering, EFileCreationMode::create_always));
					auto encoder = new_text_encoder();
					luexp(encoder->encode(var, f));
					f = nullptr;
				}
				m_res = 0;
			}
			lucatch
			{
				m_res = lures;
				if (m_res == BasicError::error_object())
				{
					m_err = get_error_object();
				}
			}
			m_finished = true;
			return;
		}

		IDispatchQueue* get_streaming_queue()
		{
			return g_dispatch;
		}
	}
}