// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Texture.cpp
* @author JXMaster
* @date 2020/5/8
*/
#include "Texture.hpp"
#include "Renderer/Renderer.hpp"

namespace luna
{
	namespace texture
	{
		void Texture::reset(const gfx::ResourceDesc& desc, SubresourceData* initial_data, uint32 num_initial_data)
		{
			auto var = new_var(EVariantType::table);
			// Push procedural request.
			auto type = new_var(EVariantType::u32);
			type->u32() = (uint32)desc.type;
			auto format = new_var(EVariantType::u32);
			format->u32() = (uint32)desc.format;
			auto width = new_var(EVariantType::u32);
			width->u32() = (uint32)desc.width;
			auto height = new_var(EVariantType::u32);
			height->u32() = (uint32)desc.height;
			auto depth_or_array_size = new_var(EVariantType::u32);
			depth_or_array_size->u32() = (uint32)desc.depth_or_array_size;
			auto access_type = new_var(EVariantType::u32);
			access_type->u32() = (uint32)desc.access_type;
			auto usages = new_var(EVariantType::u32);
			usages->u32() = (uint32)desc.usages;
			auto sample_count = new_var(EVariantType::u32);
			sample_count->u32() = (uint32)desc.sample_count;
			auto sample_quality = new_var(EVariantType::u32);
			sample_quality->u32() = (uint32)desc.sample_quality;
			auto mip_levels = new_var(EVariantType::u32);
			mip_levels->u32() = (uint32)desc.mip_levels;

			var->set_field(0, intern_name("type"), type);
			var->set_field(0, intern_name("format"), format);
			var->set_field(0, intern_name("width"), width);
			var->set_field(0, intern_name("height"), height);
			var->set_field(0, intern_name("depth_or_array_size"), depth_or_array_size);
			var->set_field(0, intern_name("access_type"), access_type);
			var->set_field(0, intern_name("usages"), usages);
			var->set_field(0, intern_name("sample_count"), sample_count);
			var->set_field(0, intern_name("sample_quality"), sample_quality);
			var->set_field(0, intern_name("mip_levels"), mip_levels);

			if (initial_data && num_initial_data)
			{
				auto init_datas = new_var1(EVariantType::variant, num_initial_data);
				for (uint32 i = 0; i < num_initial_data; ++i)
				{
					auto init_data = new_var(EVariantType::table);
					auto data = new_var(EVariantType::blob);
					data->blob() = initial_data[i].data;

					auto subresource = new_var(EVariantType::u32);
					subresource->u32() = initial_data[i].subresource;

					auto format = new_var(EVariantType::u32);
					format->u32() = (uint32)initial_data[i].format;
					auto width = new_var(EVariantType::u32);
					width->u32() = (uint32)initial_data[i].width;
					auto height = new_var(EVariantType::u32);
					height->u32() = (uint32)initial_data[i].height;
					auto depth = new_var(EVariantType::u32);
					depth->u32() = (uint32)initial_data[i].depth;
					auto row_pitch = new_var(EVariantType::u32);
					row_pitch->u32() = (uint32)initial_data[i].row_pitch;

					init_data->set_field(0, intern_name("data"), data);
					init_data->set_field(0, intern_name("subresource"), subresource);
					
					init_data->set_field(0, intern_name("format"), format);
					init_data->set_field(0, intern_name("width"), width);
					init_data->set_field(0, intern_name("height"), height);
					init_data->set_field(0, intern_name("depth"), depth);
					init_data->set_field(0, intern_name("row_pitch"), row_pitch);

					init_datas->var_buf()[i] = init_data;
				}
				var->set_field(0, intern_name("initial_data"), init_datas);
			}
			meta()->load(asset::EAssetLoadFlag::force_reload | asset::EAssetLoadFlag::procedural, var);
		}

		Unconstructed<TextureAssetType> g_texture_asset_type;

		P<asset::IAsset> TextureAssetType::on_new_asset(asset::IAssetMeta* meta)
		{
			auto ass = box_ptr(new_obj<Texture>(get_module_allocator()));
			ass->m_meta = meta;
			return ass;
		}

		static RV decode_var(IVariant* var, gfx::ResourceDesc& desc, Vector<SubresourceData>& initial_data)
		{
			lutry
			{
				lulet(type, var->field(0, intern_name("type")));
				lulet(format, var->field(0, intern_name("format")));
				lulet(width, var->field(0, intern_name("width")));
				lulet(height, var->field(0, intern_name("height")));
				lulet(depth_or_array_size, var->field(0, intern_name("depth_or_array_size")));
				lulet(access_type, var->field(0, intern_name("access_type")));
				lulet(usages, var->field(0, intern_name("usages")));
				lulet(sample_count, var->field(0, intern_name("sample_count")));
				lulet(sample_quality, var->field(0, intern_name("sample_quality")));
				lulet(mip_levels, var->field(0, intern_name("mip_levels")));

				lulet(type_var, type->check_u32());
				lulet(format_var, format->check_u32());
				lulet(access_type_var, access_type->check_u32());
				lulet(usages_var, usages->check_u32());

				using namespace gfx;

				desc.type = (EResourceType)type_var.get();
				desc.format = (EResourceFormat)format_var.get();
				desc.access_type = (EAccessType)access_type_var.get();
				desc.usages = (EResourceUsageFlag)usages_var.get();

				luset(desc.width, width->check_u32());
				luset(desc.height, height->check_u32());
				luset(desc.depth_or_array_size, depth_or_array_size->check_u32());
				luset(desc.sample_count, sample_count->check_u32());
				luset(desc.sample_quality, sample_quality->check_u32());
				luset(desc.mip_levels, mip_levels->check_u32());

				auto _init_data = var->field(0, intern_name("initial_data"));
				if (succeeded(_init_data))
				{
					auto init_data = _init_data.get();
					auto sz = init_data->length(1);
					lulet(init_data_buf, init_data->check_var_buf());
					for (size_t i = 0; i < sz; ++i)
					{
						SubresourceData data;
						lulet(data_blob, init_data_buf[i]->field(0, intern_name("data")));
						lulet(subresource, init_data_buf[i]->field(0, intern_name("subresource")));

						lulet(format, init_data_buf[i]->field(0, intern_name("format")));
						lulet(width, init_data_buf[i]->field(0, intern_name("width")));
						lulet(height, init_data_buf[i]->field(0, intern_name("height")));
						lulet(depth, init_data_buf[i]->field(0, intern_name("depth")));
						lulet(row_pitch, init_data_buf[i]->field(0, intern_name("row_pitch")));

						lulet(data_ptr, data_blob->check_blob());
						data.data = data_ptr.get();

						luset(data.subresource, subresource->check_u32());

						lulet(format_var, format->check_u32());
						data.format = (EResourceFormat)format_var.get();

						luset(data.width, width->check_u32());
						luset(data.height, height->check_u32());
						luset(data.depth, depth->check_u32());
						luset(data.row_pitch, row_pitch->check_u32());
						initial_data.push_back(data);
					}
				}
			}
			lucatchret;
			return s_ok;
		}

		static RP<gfx::IResource> create_res(IVariant* data)
		{
			using namespace gfx;
			P<IResource> res;
			lutry
			{
				ResourceDesc desc;
				Vector<SubresourceData> initial_data(get_module_allocator());
				luexp(decode_var(data, desc, initial_data));
				auto dev = renderer::device();
				luset(res, dev->new_resource(desc));
				if (!initial_data.empty())
				{
					// Upload data.
					lulet(cmdbuf, renderer::main_copy_queue()->new_command_buffer());

					// Here we pack all datas into one buffer.
					size_t upload_buffer_size = 0;
					uint32 pitch_alignment, placement_alignment;
					dev->check_feature_support(EDeviceFeature::texture_data_pitch_alignment, &pitch_alignment);
					dev->check_feature_support(EDeviceFeature::texture_data_placement_alignment, &placement_alignment);
					for (auto& i : initial_data)
					{
						upload_buffer_size = align_upper(upload_buffer_size, placement_alignment);
						size_t data_size = align_upper(i.row_pitch, pitch_alignment) * i.height * i.depth;
						upload_buffer_size += data_size;
					}

					lulet(buf, dev->new_resource(ResourceDesc::buffer(EAccessType::upload, EResourceUsageFlag::none, upload_buffer_size)));
					lulet(mapped, buf->map_subresource(0, false, 1, 0));

					size_t buf_offset = 0;

					for (auto& i : initial_data)
					{
						buf_offset = align_upper(buf_offset, placement_alignment);
						size_t copy_size_per_row = bits_per_pixel(i.format) * i.width / 8;
						size_t buf_row_pitch = align_upper(copy_size_per_row, pitch_alignment);
						size_t buf_slice_pitch = buf_row_pitch * i.height;
						size_t buf_size = buf_slice_pitch * i.depth;

						memcpy_bitmap3d((void*)((size_t)mapped + buf_offset), i.data->data(), copy_size_per_row, i.height, i.depth,
							buf_row_pitch, i.row_pitch, buf_slice_pitch, i.row_pitch * i.height);

						TextureCopyLocation srcl, destl;
						srcl.type = ETextureCopyType::placed_footprint;
						srcl.placed_footprint.offset = buf_offset;
						srcl.placed_footprint.footprint.width = i.width;
						srcl.placed_footprint.footprint.height = i.height;
						srcl.placed_footprint.footprint.depth = i.depth;
						srcl.placed_footprint.footprint.row_pitch = (uint32)buf_row_pitch;
						srcl.placed_footprint.footprint.format = i.format;
						srcl.resource = buf;
						destl.type = ETextureCopyType::subresource_index;
						destl.subresource_index = i.subresource;
						destl.resource = res;
						cmdbuf->copy_texture_region(destl, 0, 0, 0, srcl);
						buf_offset += buf_size;
					}

					buf->unmap_subresource(0, true);
					luexp(cmdbuf->submit());
					cmdbuf->wait();
				}
			}
			lucatchret;
			return res;
		}

		RV TextureAssetType::on_load_data(asset::IAsset* target_asset, IVariant* data, IVariant* params)
		{
			lutry
			{
				lulet(res, create_res(data));
				P<Texture> tex = target_asset;
				tex->m_res = res;
			}
			lucatchret;
			return s_ok;
		}

		RV TextureAssetType::on_load_procedural_data(asset::IAsset* target_asset, IVariant* params)
		{
			lutry
			{
				lulet(res, create_res(params));
				P<Texture> tex = target_asset;
				tex->m_res = res;
			}
			lucatchret;
			return s_ok;
		}

		void TextureAssetType::on_unload_data(asset::IAsset* target_asset)
		{
			P<Texture> tex = target_asset;
			tex->m_res = nullptr;
		}

		struct SubresourceInfo
		{
			size_t offset;
			gfx::EResourceFormat format;
			uint32 width;
			uint32 height;
			uint32 depth;
			uint32 row_pitch;

			uint32 rb_row_pitch;
			uint32 rb_slice_pitch;
			uint32 rb_size;
		};

		RP<IVariant> TextureAssetType::on_save_data(asset::IAsset* target_asset, IVariant* params)
		{
			using namespace gfx;
			P<Texture> tex = target_asset;
			auto var = new_var(EVariantType::table);
			// Reads the texture back.
			lutry
			{
				auto res = tex->m_res;
				auto desc = res->desc();
				size_t subres_count;
				if (desc.type == EResourceType::texture_3d)
				{
					subres_count = desc.mip_levels;
				}
				else
				{
					subres_count = desc.mip_levels * desc.depth_or_array_size;
				}

				auto dev = renderer::device();
				uint32 pitch_alignment, placement_alignment;
				dev->check_feature_support(EDeviceFeature::texture_data_pitch_alignment, &pitch_alignment);
				dev->check_feature_support(EDeviceFeature::texture_data_placement_alignment, &placement_alignment);

				// Collects information for every subresource.
				Vector<SubresourceInfo> data(get_module_allocator());
				data.reserve(subres_count);

				size_t readback_buffer_size = 0;

				for (size_t i = 0; i < subres_count; ++i)
				{
					SubresourceInfo d;
					uint32 mip, array_slice;
					calc_mip_array_slice(i, desc.mip_levels, mip, array_slice);
					d.width = max<uint32>((uint32)(desc.width >> mip), 1);
					d.height = max<uint32>(desc.height >> mip, 1);
					if (desc.type == EResourceType::texture_3d)
					{
						d.depth = max<uint32>(desc.depth_or_array_size >> mip, 1);
					}
					else
					{
						d.depth = 1;
					}
					d.format = desc.format;
					d.row_pitch = bits_per_pixel(desc.format) * d.width / 8;	// Tightly arranged.

					readback_buffer_size = align_upper(readback_buffer_size, placement_alignment);
					d.offset = readback_buffer_size;
					d.rb_row_pitch = align_upper(d.row_pitch, pitch_alignment);
					d.rb_slice_pitch = d.rb_row_pitch * d.height;
					d.rb_size = d.rb_slice_pitch * d.depth;
					readback_buffer_size += d.rb_size;

					data.push_back(d);
				}

				// Read data back.
				lulet(buf, dev->new_resource(ResourceDesc::buffer(EAccessType::readback, EResourceUsageFlag::none, readback_buffer_size)));
				lulet(cmdbuf, renderer::main_copy_queue()->new_command_buffer());

				for (size_t i = 0; i < subres_count; ++i)
				{
					TextureCopyLocation srcl, destl;

					srcl.type = ETextureCopyType::subresource_index;
					srcl.subresource_index = i;
					srcl.resource = res;

					destl.type = ETextureCopyType::placed_footprint;
					destl.placed_footprint.offset = data[i].offset;
					destl.placed_footprint.footprint.width = data[i].width;
					destl.placed_footprint.footprint.height = data[i].height;
					destl.placed_footprint.footprint.depth = data[i].depth;
					destl.placed_footprint.footprint.row_pitch = (uint32)data[i].rb_row_pitch;
					destl.placed_footprint.footprint.format = data[i].format;
					destl.resource = buf;
					cmdbuf->copy_texture_region(destl, 0, 0, 0, srcl);
				}

				luexp(cmdbuf->submit());
				cmdbuf->wait();
				lulet(mapped, buf->map_subresource(0, true));

				// Record information.
				auto type = new_var(EVariantType::u32);
				type->u32() = (uint32)desc.type;
				auto format = new_var(EVariantType::u32);
				format->u32() = (uint32)desc.format;
				auto width = new_var(EVariantType::u32);
				width->u32() = (uint32)desc.width;
				auto height = new_var(EVariantType::u32);
				height->u32() = (uint32)desc.height;
				auto depth_or_array_size = new_var(EVariantType::u32);
				depth_or_array_size->u32() = (uint32)desc.depth_or_array_size;
				auto access_type = new_var(EVariantType::u32);
				access_type->u32() = (uint32)desc.access_type;
				auto usages = new_var(EVariantType::u32);
				usages->u32() = (uint32)desc.usages;
				auto sample_count = new_var(EVariantType::u32);
				sample_count->u32() = (uint32)desc.sample_count;
				auto sample_quality = new_var(EVariantType::u32);
				sample_quality->u32() = (uint32)desc.sample_quality;
				auto mip_levels = new_var(EVariantType::u32);
				mip_levels->u32() = (uint32)desc.mip_levels;

				var->set_field(0, intern_name("type"), type);
				var->set_field(0, intern_name("format"), format);
				var->set_field(0, intern_name("width"), width);
				var->set_field(0, intern_name("height"), height);
				var->set_field(0, intern_name("depth_or_array_size"), depth_or_array_size);
				var->set_field(0, intern_name("access_type"), access_type);
				var->set_field(0, intern_name("usages"), usages);
				var->set_field(0, intern_name("sample_count"), sample_count);
				var->set_field(0, intern_name("sample_quality"), sample_quality);
				var->set_field(0, intern_name("mip_levels"), mip_levels);

				// Map data back to the blob.
				auto init_datas = new_var1(EVariantType::variant, subres_count);
				for (uint32 i = 0; i < subres_count; ++i)
				{
					auto init_data = new_var(EVariantType::table);
					auto data_blob = new_var(EVariantType::blob);

					// Copy the data one row per time.
					auto data_buf = new_memory_stream();
					size_t cursor = data[i].offset;
					for (uint32 cur_row = 0; cur_row < data[i].height; ++cur_row)
					{
						luexp(data_buf->write((void*)((size_t)mapped + cursor), data[i].row_pitch));
						cursor += data[i].rb_row_pitch;
					}
					lulet(total_size, data_buf->tell());
					luexp(data_buf->seek(0, ESeekMode::begin));

					data_blob->blob() = data_buf->buffer();

					auto subresource = new_var(EVariantType::u32);
					subresource->u32() = i;

					auto format = new_var(EVariantType::u32);
					format->u32() = (uint32)data[i].format;
					auto width = new_var(EVariantType::u32);
					width->u32() = (uint32)data[i].width;
					auto height = new_var(EVariantType::u32);
					height->u32() = (uint32)data[i].height;
					auto depth = new_var(EVariantType::u32);
					depth->u32() = (uint32)data[i].depth;
					auto row_pitch = new_var(EVariantType::u32);
					row_pitch->u32() = (uint32)data[i].row_pitch;

					init_data->set_field(0, intern_name("data"), data_blob);
					init_data->set_field(0, intern_name("subresource"), subresource);

					init_data->set_field(0, intern_name("format"), format);
					init_data->set_field(0, intern_name("width"), width);
					init_data->set_field(0, intern_name("height"), height);
					init_data->set_field(0, intern_name("depth"), depth);
					init_data->set_field(0, intern_name("row_pitch"), row_pitch);

					init_datas->var_buf()[i] = init_data;
				}
				var->set_field(0, intern_name("initial_data"), init_datas);
				buf->unmap_subresource(0, false, 1, 0);
			}
			lucatchret;
			return var;
		}

		void deinit()
		{
			auto n = g_texture_asset_type.get().m_texture_asset_name;
			auto _ = asset::unregister_asset_type(n);
		}

		LUNA_TEXTURE_API void init()
		{
			g_texture_asset_type.construct();
			auto ptr = box_ptr(&g_texture_asset_type.get());
			auto _ = asset::register_asset_type(ptr);
			add_module("Texture", deinit);
		}

		LUNA_TEXTURE_API RP<ITexture> new_texture()
		{
			auto ass = asset::new_asset(intern_name("Texture"));
			if (succeeded(ass))
			{
				P<ITexture> tex = ass.get();
				return tex;
			}
			return ass.result();
		}
	}
}