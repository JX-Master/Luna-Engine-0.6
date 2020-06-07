// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Transform.hpp
* @author JXMaster
* @date 2020/5/14
*/
#pragma once
#include "3DEngineHeader.hpp"
#include <Base/TSAssert.hpp>

namespace luna
{
	namespace e3d
	{
		class Transform : public ITransform
		{
		public:
			lucid("{850961ce-327b-4f6e-a041-503b0a8b82e5}");
			luiimpl(Transform, ITransform, scene::IComponent, ISerializable, IObject);
			lutsassert_lock();

			Tranform3D m_transform;

			WP<scene::IEntity> m_entity;
			WP<ITransform> m_parent;
			Vector<WP<ITransform>> m_children;

			Transform(IAllocator* alloc) :
				luibind(alloc),
				m_transform(Tranform3D::identity()) {}

			void set_parent(ITransform* parent_transform)
			{
				m_parent = parent_transform;
			}

			virtual RP<IVariant> serialize() override;
			virtual RV deserialize(IVariant* obj) override;
			virtual scene::IComponentType* type_object() override;
			virtual P<scene::IEntity> belonging_entity() override;
			virtual Vector<Guid> referred_assets() override
			{
				return Vector<Guid>();
			}
			virtual P<ITransform> parent() override;
			virtual Vector<P<ITransform>> children() override;
			virtual RV remove_child(ITransform* child) override;
			virtual void remove_all_children() override;
			virtual Float3 local_position() override
			{
				lutsassert();
				return m_transform.position;
			}
			virtual Quaternion local_rotation() override
			{
				lutsassert();
				return m_transform.rotation;
			}
			virtual Float3 local_scale() override
			{
				lutsassert();
				return m_transform.scale;
			}
			virtual void set_local_position(const Float3& position) override
			{
				lutsassert();
				m_transform.position = position;
			}
			virtual void set_local_rotation(const Quaternion& rotation) override
			{
				lutsassert();
				m_transform.rotation = rotation;
			}
			virtual void set_local_scale(const Float3& scale) override
			{
				lutsassert();
				m_transform.scale = scale;
			}
			virtual Float3 world_position() override;
			virtual Quaternion world_rotation() override;
			virtual void set_world_position(const Float3& position) override;
			virtual Float4x4 parent_to_this_matrix() override;
			virtual Float4x4 this_to_parent_matrix() override;
			virtual Float4x4 local_to_world_matrix() override;
			virtual Float4x4 world_to_local_matrix() override;
			virtual void set_parent_to_this_matrix(const Float4x4& mat) override;
			virtual void set_this_to_parent_matrix(const Float4x4& mat) override;
			virtual void set_local_to_world_matrix(const Float4x4& mat) override;
			virtual void set_world_to_local_matrix(const Float4x4& mat) override;
		};

		class TransformComponentType : public scene::IComponentType
		{
		public:
			lucid("{fea5b366-355d-48db-8cdc-2cb417fa2518}");
			luiimpl_static(TransformComponentType, scene::IComponentType, IObject);

			P<IName> m_type_name;

			TransformComponentType() :
				m_type_name(intern_name("Transform")) {}

			virtual IName* type_name() override
			{
				return m_type_name;
			}
			virtual RP<scene::IComponent> new_component(scene::IEntity* belonging_entity) override
			{
				auto t = box_ptr(new_obj_aligned<Transform>(get_module_allocator()));
				t->m_entity = belonging_entity;
				return t;
			}
			virtual void on_dependency_data_load(scene::IComponent* component, asset::IAsset* dependency_asset) override {}
			virtual void on_dependency_data_unload(scene::IComponent* component, asset::IAsset* dependency_asset) override {}
			virtual void on_dependency_replace(scene::IComponent* component, const Guid& before, const Guid& after) override {}
		};
		
		extern Unconstructed<TransformComponentType> g_transform_type;
	}
}