// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file AssetBrowser.hpp
* @author JXMaster
* @date 2020/4/29
*/
#pragma once
#include "StudioHeader.hpp"
#include <Base/HashSet.hpp>

namespace luna
{
	namespace editor
	{
		class MainEditor;

		//! Asset browser context.
		class AssetBrowser : public IObject
		{
		public:
			lucid("{42ca394d-2c56-4502-8023-b806b422d44f}");
			luiimpl(AssetBrowser, IObject);

			MainEditor* m_editor;

			//uint32 m_index;	// The index of the asset browser.
			//bool m_open;	// If the browser is closed.

			//----- Begin of States for navbar -----
			Vector<P<IPath>> m_histroy_paths;
			P<IMutPath> m_path;
			P<IStringBuffer> m_path_edit_text;
			uint32 m_current_location_in_histroy_path;
			bool m_is_text_editing;
			//----- End of States for navbar -----

			//----- Begin of States for tile context -----
			float32 m_tile_size;
			Float2U m_ms_mouse_begin_pos;	// ms for multi select.
			bool m_ms_is_dragging;
			HashSet<P<IName>> m_selections;
			//----- End of States for tile context -----

			AssetBrowser(IAllocator* alloc) :
				luibind(alloc),
				//m_open(true),
				m_histroy_paths(alloc),
				m_current_location_in_histroy_path(0),
				m_is_text_editing(false),
				m_tile_size(128),
				m_selections(alloc),
				m_ms_is_dragging(false)
			{
				m_path_edit_text = new_string_buffer();
			}

			void change_path(IPath* path);

			void render();

		private:
			void navbar();
			void tile_context();
		};
	}
}