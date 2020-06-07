// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file MainEditor.hpp
* @author JXMaster
* @date 2020/4/27
*/
#pragma once
#include "StudioHeader.hpp"
#include "AssetBrowser.hpp"
#include <Base/HashMap.hpp>
#include "IAssetEditorType.hpp"
#include "IAssetImporterType.hpp"

namespace luna
{
	namespace editor
	{
		class MainEditor : public IObject
		{
		public:
			lucid("{23a38a78-1b58-4c5f-8be1-25f258d7fdf3}");
			luiimpl(MainEditor, IObject);

			P<IPath> m_project_path;

			P<gfx::IWindow> m_window;
			P<gfx::ISwapChain> m_swap_chain;
			P<gfx::ICommandBuffer> m_cmdbuf;
			P<gfx::IResource> m_back_buffer;
			P<gfx::IRenderPass> m_render_pass;
			P<gfx::IFrameBuffer> m_back_buffer_fbo;

			P<ILogger> m_logger;

			P<imgui::IContext> m_ctx;

			P<AssetBrowser> m_asset_browser;

			HashMap<P<IName>, P<IAssetImporterType>> m_importer_types;

			HashMap<P<IName>, P<IAssetEditorType>> m_editor_types;

			Vector<P<IAssetEditor>> m_editors;

			//uint32 m_next_asset_browser_index;

			bool m_exiting;

			uint32 m_main_window_width;
			uint32 m_main_window_height;

			MainEditor(IAllocator* alloc) :
				luibind(alloc),
				m_importer_types(alloc),
				m_editor_types(alloc),
				m_editors(alloc),
				m_exiting(false),
				m_main_window_width(0),
				m_main_window_height(0) {}
				//m_next_asset_browser_index(0) {}

			RV init(IPath* project_path);

			void register_asset_importer_type(IAssetImporterType* factory)
			{
				auto name = factory->name();
				m_importer_types.insert(Pair<P<IName>, P<IAssetImporterType>>(name, factory));
			}

			void register_asset_editor_type(IAssetEditorType* factory)
			{
				auto name = factory->asset_type();
				m_editor_types.insert(Pair<P<IName>, P<IAssetEditorType>>(name, factory));
			}

			RV update();

			//P<AssetBrowser> new_asset_browser(P<IPath> initial_path);
		};

		void run_main_editor(IPath* project_path);

		extern MainEditor* g_main_editor;
	}
}