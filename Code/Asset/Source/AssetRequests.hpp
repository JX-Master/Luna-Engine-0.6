// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file AssetRequest.hpp
* @author JXMaster
* @date 2020/4/18
*/
#pragma once
#include "AssetHeader.hpp"
#include <Core/Interface.hpp>

namespace Luna
{
	namespace Asset
	{
		class AssetLoadRequest : public IRunnable
		{
		public:
			lucid("{548b7f7e-b03c-4148-9b0c-f2f0f4218eb5}");
			luiimpl(AssetLoadRequest, IRunnable, IObject);

			WP<IAsset> m_asset;
			Variant m_params;
			EAssetLoadFlag m_flags;

			AssetLoadRequest() {}

			virtual void run() override;
		};

		class AssetSaveRequest : public IAssetSaveRequest, public IRunnable
		{
		public:
			lucid("{84260b76-5fe3-4ec9-b105-d56b44986870}");
			luqbegin();
			luqitem(this, AssetSaveRequest, IRunnable);
			luqitem((IAssetSaveRequest*)this, IAssetSaveRequest, IWaitable, IObject);
			luqend();
			lurc();

			WP<IAsset> m_asset;
			Error m_err;
			Variant m_params;
			errcode_t m_res;
			bool m_save_data;
			volatile bool m_finished;
			EAssetSaveFormat m_format;

			AssetSaveRequest() :
				m_finished(false),
				m_res(0) {}

			virtual void wait() override
			{
				if (!m_finished)
				{
					yield_current_thread();
				}
			}
			virtual RV try_wait() override
			{
				return m_finished ? RV() : BasicError::timeout();
			}

			virtual void run() override;
			virtual errcode_t result() override
			{
				return m_res;
			}
			virtual Error& error_object() override
			{
				return m_err;
			}
		};
	}
}