// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file MainEditor.hpp
* @author JXMaster
* @date 2020/4/27
*/
#pragma once
#include "StudioHeader.hpp"
#include "AssetBrowser.hpp"
#include <Runtime/HashMap.hpp>
#include "IAssetEditorType.hpp"
#include "IAssetImporterType.hpp"

namespace Luna
{
	namespace editor
	{
		class MainEditor : public IObject
		{
		public:
			lucid("{23a38a78-1b58-4c5f-8be1-25f258d7fdf3}");
			luiimpl(MainEditor, IObject);

			Path m_project_path;

			P<Gfx::IWindow> m_window;
			P<Gfx::ISwapChain> m_swap_chain;
			P<Gfx::ICommandBuffer> m_cmdbuf;
			P<Gfx::IResource> m_back_buffer;
			P<Gfx::IRenderPass> m_render_pass;
			P<Gfx::IFrameBuffer> m_back_buffer_fbo;

			P<ImGui::IContext> m_ctx;

			P<AssetBrowser> m_asset_browser;

			HashMap<Name, P<IAssetImporterType>> m_importer_types;

			HashMap<Name, P<IAssetEditorType>> m_editor_types;

			Vector<P<IAssetEditor>> m_editors;

			//u32 m_next_asset_browser_index;

			bool m_exiting;

			u32 m_main_window_width;
			u32 m_main_window_height;

			MainEditor() :
				m_exiting(false),
				m_main_window_width(0),
				m_main_window_height(0) {}
				//m_next_asset_browser_index(0) {}

			RV init(const Path& project_path);

			void register_asset_importer_type(IAssetImporterType* factory)
			{
				auto name = factory->name();
				m_importer_types.insert(Pair<Name, P<IAssetImporterType>>(name, factory));
			}

			void register_asset_editor_type(IAssetEditorType* factory)
			{
				auto name = factory->asset_type();
				m_editor_types.insert(Pair<Name, P<IAssetEditorType>>(name, factory));
			}

			RV update();

			//P<AssetBrowser> new_asset_browser(Path initial_path);
		};

		void run_main_editor(const Path& project_path);

		extern MainEditor* g_main_editor;
	}
}