// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ObjFile.hpp
* @author JXMaster
* @date 2020/5/12
*/
#include "ObjLoaderHeader.hpp"
#include <Core/Interface.hpp>

namespace Luna
{
	namespace obj_loader
	{
		class ObjFile : public IObjFile
		{
		public:
			lucid("{c4824041-ff1a-4fe0-a374-9554ccde8963}");
			luiimpl(ObjFile, IObjFile, IObject);

			Attributes m_attributes;
			Vector<Shape> m_shapes;

			ObjFile() {}

			virtual Attributes& attributes() override
			{
				return m_attributes;
			}
			virtual Vector<Shape>& shapes() override
			{
				return m_shapes;
			}
		};
	}
}