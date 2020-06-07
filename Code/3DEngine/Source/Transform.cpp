// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Transform.cpp
* @author JXMaster
* @date 2020/5/14
*/
#pragma once
#include "Transform.hpp"

namespace luna
{
	namespace e3d
	{
		Unconstructed<TransformComponentType> g_transform_type;

		RP<IVariant> Transform::serialize()
		{
			lutsassert();
			P<IVariant> data = new_var(EVariantType::table);
			auto pos = new_var1(EVariantType::f32, 3);
			auto pos_data = pos->f32_buf();
			pos_data[0] = m_transform.position.x;
			pos_data[1] = m_transform.position.y;
			pos_data[2] = m_transform.position.z;
			auto rot = new_var1(EVariantType::f32, 4);
			auto rot_data = rot->f32_buf();
			rot_data[0] = m_transform.rotation.x;
			rot_data[1] = m_transform.rotation.y;
			rot_data[2] = m_transform.rotation.z;
			rot_data[3] = m_transform.rotation.w;
			auto scale = new_var1(EVariantType::f32, 3);
			auto scale_data = scale->f32_buf();
			scale_data[0] = m_transform.scale.x;
			scale_data[1] = m_transform.scale.y;
			scale_data[2] = m_transform.scale.z;
			data->set_field(0, intern_name("position"), pos);
			data->set_field(0, intern_name("rotation"), rot);
			data->set_field(0, intern_name("scale"), scale);

			// Only save children.
			Vector<P<scene::IEntity>> es;
			for (auto& i : m_children)
			{
				auto t = i.lock();
				auto p = t->belonging_entity();
				if (p)
				{
					es.push_back(p);
				}
			}
			if (!es.empty())
			{
				auto child_ents = new_var1(EVariantType::name, es.size());
				auto ents_data = child_ents->name_buf();
				for (size_t i = 0; i < es.size(); ++i)
				{
					ents_data[i] = es[i]->name();
				}
				data->set_field(0, intern_name("children"), child_ents);
			}
			return data;
		}

		RV Transform::deserialize(IVariant* obj)
		{
			lutsassert();
			lutry
			{
				lulet(pos, obj->field(0, intern_name("position")));
				lulet(rot, obj->field(0, intern_name("rotation")));
				lulet(scale, obj->field(0, intern_name("scale")));
				lulet(pos_data, pos->check_f32_buf());
				lulet(rot_data, rot->check_f32_buf());
				lulet(scale_data, scale->check_f32_buf());
				m_transform.position.x = pos_data[0];
				m_transform.position.y = pos_data[1];
				m_transform.position.z = pos_data[2];
				m_transform.rotation.x = rot_data[0];
				m_transform.rotation.y = rot_data[1];
				m_transform.rotation.z = rot_data[2];
				m_transform.rotation.w = rot_data[3];
				m_transform.scale.x = scale_data[0];
				m_transform.scale.y = scale_data[1];
				m_transform.scale.z = scale_data[2];

				auto _child_ents = obj->field(0, intern_name("children"));
				if (succeeded(_child_ents))
				{
					auto e = m_entity.lock();
					auto s = e->belonging_scene();
					auto child_ents = _child_ents.get();
					size_t num_children = child_ents->length(1);
					lulet(enames, child_ents->check_name_buf());
					for (size_t i = 0; i < num_children; ++i)
					{
						auto childe = s->find_entity(enames[i]);
						if (failed(childe))
						{
							continue;
						}
						auto childt = childe.get()->get_component(intern_name("Transform"));
						if (failed(childt))
						{
							continue;
						}
						P<ITransform> t = childt.get();
						t.as<Transform>()->set_parent(this);
						m_children.push_back(WP<ITransform>(t));
					}
				}
			}
			lucatchret;
			return s_ok;
		}

		scene::IComponentType* Transform::type_object()
		{
			return &g_transform_type.get();
		}

		P<scene::IEntity> Transform::belonging_entity()
		{
			return m_entity.lock();
		}

		P<ITransform> Transform::parent()
		{
			lutsassert();
			return m_parent.lock();
		}

		Vector<P<ITransform>> Transform::children()
		{
			lutsassert();
			Vector<P<ITransform>> r;
			for (auto& i : m_children)
			{
				auto t = i.lock();
				if (t)
				{
					r.push_back(move(t));
				}
			}
			return r;
		}

		RV Transform::remove_child(ITransform* child)
		{
			lutsassert();
			auto iter = m_children.begin();
			while (iter != m_children.end())
			{
				auto t = iter->lock();
				if (!t)
				{
					++iter;
					continue;
				}
				if (t.get() == child)
				{
					t.as<Transform>()->set_parent(nullptr);
					m_children.erase(iter);
					return s_ok;
				}
				++iter;
			}
			return e_item_not_exist;
		}

		void Transform::remove_all_children()
		{
			lutsassert();
			for (auto& i : m_children)
			{
				auto t = i.lock();
				if (i)
				{
					t.as<Transform>()->set_parent(nullptr);
				}
			}
			m_children.clear();
		}

		Float3 Transform::world_position()
		{
			auto p = m_parent.lock();
			if (p)
			{
				Float4x4 mat = p->local_to_world_matrix();
				Float4 pos = mul(Float4(m_transform.position.x, m_transform.position.y, m_transform.position.z, 1.0f), mat);
				return Float3(pos.x, pos.y, pos.z);
			}
			return m_transform.position;
		}

		Quaternion Transform::world_rotation()
		{
			auto p = m_parent.lock();
			if (p)
			{
				return m_transform.rotation * p->world_rotation();
			}
			return m_transform.rotation;
		}

		void Transform::set_world_position(const Float3& position)
		{
			Float4 pos(position.x, position.y, position.z, 1.0f);
			pos = mul(pos, world_to_local_matrix());
			set_local_position(Float3(pos.x, pos.y, pos.z));
		}

		Float4x4 Transform::parent_to_this_matrix()
		{
			return inverse(this_to_parent_matrix());
		}

		Float4x4 Transform::this_to_parent_matrix()
		{
			return Float4x4::make_affine_position_rotation_scale(m_transform.position, m_transform.rotation, m_transform.scale);
		}

		Float4x4 Transform::local_to_world_matrix()
		{
			auto p = m_parent.lock();
			if (p)
			{
				return mul(this_to_parent_matrix(), p->local_to_world_matrix());
			}
			else
			{
				return this_to_parent_matrix();
			}
		}

		Float4x4 Transform::world_to_local_matrix()
		{
			auto p = m_parent.lock();
			if (p)
			{
				return mul(p->world_to_local_matrix(), p->parent_to_this_matrix());
			}
			else
			{
				return parent_to_this_matrix();
			}
		}

		void Transform::set_parent_to_this_matrix(const Float4x4& mat)
		{
			set_this_to_parent_matrix(inverse(mat));
		}

		void Transform::set_this_to_parent_matrix(const Float4x4& mat)
		{
			set_local_position(mat.translation());
			set_local_scale(mat.scale_factor());
			Float3 eular_angles = mat.rotation_matrix().euler_angles();
			set_local_rotation(Quaternion::from_roll_pitch_yaw(eular_angles.x, eular_angles.y, eular_angles.z));
		}

		void Transform::set_local_to_world_matrix(const Float4x4& mat)
		{
			auto p = m_parent.lock();
			if (p)
			{
				// We need to decompose the local-to-world matrix to this-to-parent matrix.
				set_this_to_parent_matrix(mul(mat, p->world_to_local_matrix()));
			}
			else
			{
				set_this_to_parent_matrix(mat);
			}
		}

		void Transform::set_world_to_local_matrix(const Float4x4& mat)
		{
			auto p = m_parent.lock();
			if (p)
			{
				set_parent_to_this_matrix(mul(p->local_to_world_matrix(), mat));
			}
			else
			{
				set_parent_to_this_matrix(mat);
			}
		}

	}
}