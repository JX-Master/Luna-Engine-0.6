// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file AssetMeta.hpp
* @author JXMaster
* @date 2020/4/17
*/
#pragma once
#include "AssetHeader.hpp"
#include <Core/Interface.hpp>
#include <Runtime/HashMap.hpp>

namespace Luna
{
	namespace Asset
	{
		class AssetMeta : public IAssetMeta
		{
		public:
			lucid("{17e6acac-c59d-4956-b9b0-11d2402e803d}");
			luiimpl(AssetMeta, IAssetMeta, IObject);

			EAssetState m_state;
			EAssetFlag m_flags;
			volatile u32 m_pin_count;

			Guid m_guid;
			WP<IAsset> m_asset;
			Name m_type;
			Path m_data_path;
			Path m_meta_path;
			Error m_error;
			P<IMutex> m_mtx;
			P<IAssetType> m_type_obj;

			Vector<Guid> m_dependencies;
			Vector<Guid> m_dependents;

#ifdef LUNA_PROFILE
			bool m_valid;		// Check if a non-valid asset is used.
#endif

			AssetMeta() :
				m_state(EAssetState::unloaded),
				m_flags(EAssetFlag::none),
#ifdef LUNA_PROFILE
				m_valid(true),
#endif
				m_pin_count(0)
			{
				m_mtx = new_mutex();
			}

			void remove_dependent(const Guid& guid);

			virtual Guid guid() override
			{
				return m_guid;
			}
			virtual Name type() override
			{
				return m_type;
			}
			virtual P<IAsset> attaching_asset() override
			{
				return m_asset.lock();
			}
			virtual EAssetState state() override
			{
				return m_state;
			}
			void internal_set_state(EAssetState state)
			{
				m_state = state;
			}
			virtual EAssetFlag flags() override
			{
				return m_flags;
			}
			void internal_set_flags(EAssetFlag flags)
			{
				m_flags = flags;
			}
			
			virtual const Path& data_path() override
			{
				return m_data_path;
			}
			virtual const Path& meta_path() override
			{
				return m_meta_path;
			}
			virtual void set_data_path(const Path& path) override;
			virtual RV set_meta_path(const Path& path) override;
			virtual Error& error_object() override
			{
				return m_error;
			}
			virtual IMutex* mutex() override
			{
				return m_mtx;
			}
			bool unused() override;
			virtual void pin() override
			{
				atom_inc_u32(&m_pin_count);
			}
			virtual void unpin() override
			{
				atom_dec_u32(&m_pin_count);
			}
			virtual u32 pin_count() override
			{
				return m_pin_count;
			}
			virtual void load(EAssetLoadFlag flags = EAssetLoadFlag::none, const Variant& params = Variant()) override;
			virtual void unload(EAssetUnloadFlag flags = EAssetUnloadFlag::none) override;
			virtual RP<IAssetSaveRequest> save_data(EAssetSaveFormat save_format, const Variant& params = Variant()) override;
			virtual RP<IAssetSaveRequest> save_meta(EAssetSaveFormat save_format) override;
			virtual Vector<Guid> dependencies() override
			{
				MutexGuard g(m_mtx);
				return m_dependencies;
			}
			virtual void internal_add_dependency(const Guid& guid) override;
			virtual bool internal_remove_dependency(const Guid& guid) override;
			virtual Vector<Guid> dependents() override
			{
				MutexGuard g(m_mtx);
				return m_dependents;
			}
			virtual RV replace_dependency(const Guid& before, const Guid& after) override;
			virtual void internal_remove_all_dependencies() override;
		};
	}
}