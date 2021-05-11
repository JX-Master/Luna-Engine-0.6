// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Texture.cpp
* @author JXMaster
* @date 2020/5/8
*/
#include "Texture.hpp"
#include "Renderer/Renderer.hpp"
#include <Runtime/Module.hpp>

namespace Luna
{
	namespace Texture
	{
		void Texture::reset(const Gfx::ResourceDesc& desc, SubresourceData* initial_data, u32 num_initial_data)
		{
			auto var = Variant(EVariantType::table);
			// Push procedural request.
			auto type = Variant(EVariantType::u32);
			type.to_u32() = (u32)desc.type;
			auto format = Variant(EVariantType::u32);
			format.to_u32() = (u32)desc.format;
			auto width = Variant(EVariantType::u32);
			width.to_u32() = (u32)desc.width;
			auto height = Variant(EVariantType::u32);
			height.to_u32() = (u32)desc.height;
			auto depth_or_array_size = Variant(EVariantType::u32);
			depth_or_array_size.to_u32() = (u32)desc.depth_or_array_size;
			auto access_type = Variant(EVariantType::u32);
			access_type.to_u32() = (u32)desc.access_type;
			auto usages = Variant(EVariantType::u32);
			usages.to_u32() = (u32)desc.usages;
			auto sample_count = Variant(EVariantType::u32);
			sample_count.to_u32() = (u32)desc.sample_count;
			auto sample_quality = Variant(EVariantType::u32);
			sample_quality.to_u32() = (u32)desc.sample_quality;
			auto mip_levels = Variant(EVariantType::u32);
			mip_levels.to_u32() = (u32)desc.mip_levels;

			var.set_field(0, u8"type", type);
			var.set_field(0, u8"format", format);
			var.set_field(0, u8"width", width);
			var.set_field(0, u8"height", height);
			var.set_field(0, u8"depth_or_array_size", depth_or_array_size);
			var.set_field(0, u8"access_type", access_type);
			var.set_field(0, u8"usages", usages);
			var.set_field(0, u8"sample_count", sample_count);
			var.set_field(0, u8"sample_quality", sample_quality);
			var.set_field(0, u8"mip_levels", mip_levels);

			if (initial_data && num_initial_data)
			{
				auto init_datas = Variant(EVariantType::variant, num_initial_data);
				for (u32 i = 0; i < num_initial_data; ++i)
				{
					auto init_data = Variant(EVariantType::table);
					auto data = Variant(EVariantType::blob);
					data.to_blob() = initial_data[i].data;

					auto subresource = Variant(EVariantType::u32);
					subresource.to_u32() = initial_data[i].subresource;

					auto format = Variant(EVariantType::u32);
					format.to_u32() = (u32)initial_data[i].format;
					auto width = Variant(EVariantType::u32);
					width.to_u32() = (u32)initial_data[i].width;
					auto height = Variant(EVariantType::u32);
					height.to_u32() = (u32)initial_data[i].height;
					auto depth = Variant(EVariantType::u32);
					depth.to_u32() = (u32)initial_data[i].depth;
					auto row_pitch = Variant(EVariantType::u32);
					row_pitch.to_u32() = (u32)initial_data[i].row_pitch;

					init_data.set_field(0, u8"data", data);
					init_data.set_field(0, u8"subresource", subresource);
					
					init_data.set_field(0, u8"format", format);
					init_data.set_field(0, u8"width", width);
					init_data.set_field(0, u8"height", height);
					init_data.set_field(0, u8"depth", depth);
					init_data.set_field(0, u8"row_pitch", row_pitch);

					init_datas.to_var_buf()[i] = init_data;
				}
				var.set_field(0, u8"initial_data", init_datas);
			}
			meta()->load(Asset::EAssetLoadFlag::force_reload | Asset::EAssetLoadFlag::procedural, var);
		}

		Unconstructed<TextureAssetType> g_texture_asset_type;

		P<Asset::IAsset> TextureAssetType::on_new_asset(Asset::IAssetMeta* meta)
		{
			auto ass = newobj<Texture>();
			ass->m_meta = meta;
			return ass;
		}

		static RV decode_var(const Variant& var, Gfx::ResourceDesc& desc, Vector<SubresourceData>& initial_data)
		{
			lutry
			{
				auto& type = var.field(0, u8"type");
				auto& format = var.field(0, u8"format");
				auto& width = var.field(0, u8"width");
				auto& height = var.field(0, u8"height");
				auto& depth_or_array_size = var.field(0, u8"depth_or_array_size");
				auto& access_type = var.field(0, u8"access_type");
				auto& usages = var.field(0, u8"usages");
				auto& sample_count = var.field(0, u8"sample_count");
				auto& sample_quality = var.field(0, u8"sample_quality");
				auto& mip_levels = var.field(0, u8"mip_levels");

				lulet(type_var, type.check_u32());
				lulet(format_var, format.check_u32());
				lulet(access_type_var, access_type.check_u32());
				lulet(usages_var, usages.check_u32());

				using namespace Gfx;

				desc.type = (EResourceType)type_var.get();
				desc.format = (EResourceFormat)format_var.get();
				desc.access_type = (EAccessType)access_type_var.get();
				desc.usages = (EResourceUsageFlag)usages_var.get();

				luset(desc.width, width.check_u32());
				luset(desc.height, height.check_u32());
				luset(desc.depth_or_array_size, depth_or_array_size.check_u32());
				luset(desc.sample_count, sample_count.check_u32());
				luset(desc.sample_quality, sample_quality.check_u32());
				luset(desc.mip_levels, mip_levels.check_u32());

				auto& init_data = var.field(0, u8"initial_data");
				auto sz = init_data.length(1);
				lulet(init_data_buf, init_data.check_var_buf());
				for (usize i = 0; i < sz; ++i)
				{
					SubresourceData data;
					auto& data_blob = init_data_buf[i].field(0, u8"data");
					auto& subresource = init_data_buf[i].field(0, u8"subresource");

					auto& format = init_data_buf[i].field(0, u8"format");
					auto& width = init_data_buf[i].field(0, u8"width");
					auto& height = init_data_buf[i].field(0, u8"height");
					auto& depth = init_data_buf[i].field(0, u8"depth");
					auto& row_pitch = init_data_buf[i].field(0, u8"row_pitch");

					lulet(data_ptr, data_blob.check_blob());
					data.data = data_ptr.get();

					luset(data.subresource, subresource.check_u32());

					lulet(format_var, format.check_u32());
					data.format = (EResourceFormat)format_var.get();

					luset(data.width, width.check_u32());
					luset(data.height, height.check_u32());
					luset(data.depth, depth.check_u32());
					luset(data.row_pitch, row_pitch.check_u32());
					initial_data.push_back(data);
				}
			}
			lucatchret;
			return RV();
		}

		static RP<Gfx::IResource> create_res(const Variant& data)
		{
			using namespace Gfx;
			P<IResource> res;
			lutry
			{
				ResourceDesc desc;
				Vector<SubresourceData> initial_data;
				luexp(decode_var(data, desc, initial_data));
				auto dev = Renderer::device();
				luset(res, dev->new_resource(desc));
				if (!initial_data.empty())
				{
					// Upload data.
					lulet(cmdbuf, Renderer::main_copy_queue()->new_command_buffer());

					// Here we pack all data into one buffer.
					usize upload_buffer_size = 0;
					u32 pitch_alignment, placement_alignment;
					dev->check_feature_support(EDeviceFeature::texture_data_pitch_alignment, &pitch_alignment);
					dev->check_feature_support(EDeviceFeature::texture_data_placement_alignment, &placement_alignment);
					for (auto& i : initial_data)
					{
						upload_buffer_size = align_upper(upload_buffer_size, placement_alignment);
						usize data_size = align_upper(i.row_pitch, pitch_alignment) * i.height * i.depth;
						upload_buffer_size += data_size;
					}

					lulet(buf, dev->new_resource(ResourceDesc::buffer(EAccessType::upload, EResourceUsageFlag::none, upload_buffer_size)));
					lulet(mapped, buf->map_subresource(0, false, 1, 0));

					usize buf_offset = 0;

					for (auto& i : initial_data)
					{
						buf_offset = align_upper(buf_offset, placement_alignment);
						usize copy_size_per_row = bits_per_pixel(i.format) * i.width / 8;
						usize buf_row_pitch = align_upper(copy_size_per_row, pitch_alignment);
						usize buf_slice_pitch = buf_row_pitch * i.height;
						usize buf_size = buf_slice_pitch * i.depth;

						memcpy_bitmap3d((void*)((usize)mapped + buf_offset), i.data.data(), copy_size_per_row, i.height, i.depth,
							buf_row_pitch, i.row_pitch, buf_slice_pitch, i.row_pitch * i.height);

						TextureCopyLocation srcl, destl;
						srcl.type = ETextureCopyType::placed_footprint;
						srcl.placed_footprint.offset = buf_offset;
						srcl.placed_footprint.footprint.width = i.width;
						srcl.placed_footprint.footprint.height = i.height;
						srcl.placed_footprint.footprint.depth = i.depth;
						srcl.placed_footprint.footprint.row_pitch = (u32)buf_row_pitch;
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

		RV TextureAssetType::on_load_data(Asset::IAsset* target_asset, const Variant& data, const Variant& params)
		{
			lutry
			{
				lulet(res, create_res(data));
				P<Texture> tex = target_asset;
				tex->m_res = res;
			}
			lucatchret;
			return RV();
		}

		RV TextureAssetType::on_load_procedural_data(Asset::IAsset* target_asset, const Variant& params)
		{
			lutry
			{
				lulet(res, create_res(params));
				P<Texture> tex = target_asset;
				tex->m_res = res;
			}
			lucatchret;
			return RV();
		}

		void TextureAssetType::on_unload_data(Asset::IAsset* target_asset)
		{
			P<Texture> tex = target_asset;
			tex->m_res = nullptr;
		}

		struct SubresourceInfo
		{
			usize offset;
			Gfx::EResourceFormat format;
			u32 width;
			u32 height;
			u32 depth;
			u32 row_pitch;

			u32 rb_row_pitch;
			u32 rb_slice_pitch;
			u32 rb_size;
		};

		R<Variant> TextureAssetType::on_save_data(Asset::IAsset* target_asset, const Variant& params)
		{
			using namespace Gfx;
			P<Texture> tex = target_asset;
			auto var = Variant(EVariantType::table);
			// Reads the texture back.
			lutry
			{
				auto res = tex->m_res;
				auto desc = res->desc();
				u32 subres_count;
				if (desc.type == EResourceType::texture_3d)
				{
					subres_count = desc.mip_levels;
				}
				else
				{
					subres_count = desc.mip_levels * desc.depth_or_array_size;
				}

				auto dev = Renderer::device();
				u32 pitch_alignment, placement_alignment;
				dev->check_feature_support(EDeviceFeature::texture_data_pitch_alignment, &pitch_alignment);
				dev->check_feature_support(EDeviceFeature::texture_data_placement_alignment, &placement_alignment);

				// Collects information for every subresource.
				Vector<SubresourceInfo> data;
				data.reserve(subres_count);

				usize readback_buffer_size = 0;

				for (u32 i = 0; i < subres_count; ++i)
				{
					SubresourceInfo d;
					u32 mip, array_slice;
					calc_mip_array_slice(i, desc.mip_levels, mip, array_slice);
					d.width = max<u32>((u32)(desc.width >> mip), 1);
					d.height = max<u32>(desc.height >> mip, 1);
					if (desc.type == EResourceType::texture_3d)
					{
						d.depth = max<u32>(desc.depth_or_array_size >> mip, 1);
					}
					else
					{
						d.depth = 1;
					}
					d.format = desc.format;
					d.row_pitch = bits_per_pixel(desc.format) * d.width / 8;	// Tightly arranged.

					readback_buffer_size = align_upper(readback_buffer_size, placement_alignment);
					d.offset = readback_buffer_size;
					d.rb_row_pitch = (u32)align_upper(d.row_pitch, pitch_alignment);
					d.rb_slice_pitch = d.rb_row_pitch * d.height;
					d.rb_size = d.rb_slice_pitch * d.depth;
					readback_buffer_size += d.rb_size;

					data.push_back(d);
				}

				// Read data back.
				lulet(buf, dev->new_resource(ResourceDesc::buffer(EAccessType::readback, EResourceUsageFlag::none, readback_buffer_size)));
				lulet(cmdbuf, Renderer::main_copy_queue()->new_command_buffer());

				for (u32 i = 0; i < subres_count; ++i)
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
					destl.placed_footprint.footprint.row_pitch = (u32)data[i].rb_row_pitch;
					destl.placed_footprint.footprint.format = data[i].format;
					destl.resource = buf;
					cmdbuf->copy_texture_region(destl, 0, 0, 0, srcl);
				}

				luexp(cmdbuf->submit());
				cmdbuf->wait();
				lulet(mapped, buf->map_subresource(0, true));

				// Record information.
				auto type = Variant(EVariantType::u32);
				type.to_u32() = (u32)desc.type;
				auto format = Variant(EVariantType::u32);
				format.to_u32() = (u32)desc.format;
				auto width = Variant(EVariantType::u32);
				width.to_u32() = (u32)desc.width;
				auto height = Variant(EVariantType::u32);
				height.to_u32() = (u32)desc.height;
				auto depth_or_array_size = Variant(EVariantType::u32);
				depth_or_array_size.to_u32() = (u32)desc.depth_or_array_size;
				auto access_type = Variant(EVariantType::u32);
				access_type.to_u32() = (u32)desc.access_type;
				auto usages = Variant(EVariantType::u32);
				usages.to_u32() = (u32)desc.usages;
				auto sample_count = Variant(EVariantType::u32);
				sample_count.to_u32() = (u32)desc.sample_count;
				auto sample_quality = Variant(EVariantType::u32);
				sample_quality.to_u32() = (u32)desc.sample_quality;
				auto mip_levels = Variant(EVariantType::u32);
				mip_levels.to_u32() = (u32)desc.mip_levels;

				var.set_field(0, u8"type", type);
				var.set_field(0, u8"format", format);
				var.set_field(0, u8"width", width);
				var.set_field(0, u8"height", height);
				var.set_field(0, u8"depth_or_array_size", depth_or_array_size);
				var.set_field(0, u8"access_type", access_type);
				var.set_field(0, u8"usages", usages);
				var.set_field(0, u8"sample_count", sample_count);
				var.set_field(0, u8"sample_quality", sample_quality);
				var.set_field(0, u8"mip_levels", mip_levels);

				// Map data back to the blob.
				auto init_datas = Variant(EVariantType::variant, subres_count);
				for (u32 i = 0; i < subres_count; ++i)
				{
					auto init_data = Variant(EVariantType::table);
					auto data_blob = Variant(EVariantType::blob);

					// Copy the data one row per time.
					auto data_buf = new_memory_stream();
					usize cursor = data[i].offset;
					for (u32 cur_row = 0; cur_row < data[i].height; ++cur_row)
					{
						luexp(data_buf->write((void*)((usize)mapped + cursor), data[i].row_pitch));
						cursor += data[i].rb_row_pitch;
					}
					lulet(total_size, data_buf->tell());
					luexp(data_buf->seek(0, ESeekMode::begin));

					data_blob.to_blob() = data_buf->get_blob();

					auto subresource = Variant(EVariantType::u32);
					subresource.to_u32() = i;

					auto format = Variant(EVariantType::u32);
					format.to_u32() = (u32)data[i].format;
					auto width = Variant(EVariantType::u32);
					width.to_u32() = (u32)data[i].width;
					auto height = Variant(EVariantType::u32);
					height.to_u32() = (u32)data[i].height;
					auto depth = Variant(EVariantType::u32);
					depth.to_u32() = (u32)data[i].depth;
					auto row_pitch = Variant(EVariantType::u32);
					row_pitch.to_u32() = (u32)data[i].row_pitch;

					init_data.set_field(0, u8"data", data_blob);
					init_data.set_field(0, u8"subresource", subresource);

					init_data.set_field(0, u8"format", format);
					init_data.set_field(0, u8"width", width);
					init_data.set_field(0, u8"height", height);
					init_data.set_field(0, u8"depth", depth);
					init_data.set_field(0, u8"row_pitch", row_pitch);

					init_datas.to_var_buf()[i] = init_data;
				}
				var.set_field(0, u8"initial_data", init_datas);
				buf->unmap_subresource(0, false, 1, 0);
			}
			lucatchret;
			return var;
		}

		void deinit()
		{
			auto n = g_texture_asset_type.get().m_texture_asset_name;
			auto _ = Asset::unregister_asset_type(n);
		}

		RV init()
		{
			g_texture_asset_type.construct();
			auto ptr = box_ptr(&g_texture_asset_type.get());
			auto _ = Asset::register_asset_type(ptr);
			return RV();
		}

		StaticRegisterModule m("Texture", "Asset;Core;Gfx;Image;Input;Renderer", init, deinit);

		LUNA_TEXTURE_API RP<ITexture> new_texture()
		{
			auto ass = Asset::new_asset(u8"Texture");
			if (succeeded(ass))
			{
				P<ITexture> tex = ass.get();
				return tex;
			}
			return ass.errcode();
		}
	}
}