// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file TextureImporter.cpp
* @author JXMaster
* @date 2020/5/8
*/
#include "TextureImporter.hpp"
namespace Luna
{
	namespace editor
	{
		RV TextureImporterType::init()
		{
			using namespace Gfx;
			lutry
			{
				ShaderInputGroupDesc groups[] = {
						ShaderInputGroupDesc(EShaderInputGroupType::cbv, 0, 1, EShaderVisibility::all),
						ShaderInputGroupDesc(EShaderInputGroupType::srv, 0, 1, EShaderVisibility::all),
						ShaderInputGroupDesc(EShaderInputGroupType::uav, 0, 1, EShaderVisibility::all),
						ShaderInputGroupDesc(EShaderInputGroupType::sampler, 0, 1, EShaderVisibility::all)
				};
				lulet(slayout, Renderer::device()->new_shader_input_layout(ShaderInputLayoutDesc(groups, 4,
					EShaderInputLayoutFlag::deny_vertex_shader_access |
					EShaderInputLayoutFlag::deny_domain_shader_access |
					EShaderInputLayoutFlag::deny_geometry_shader_access |
					EShaderInputLayoutFlag::deny_hull_shader_access |
					EShaderInputLayoutFlag::deny_pixel_shader_access)));

				lulet(psf, platform_open_file("MipmapGenerationCS.cso", EFileOpenFlag::read, EFileCreationMode::open_existing));
				auto file_size = psf->size();
				auto cs_blob = Blob((usize)file_size);
				luexp(psf->read(cs_blob.data(), cs_blob.size()));
				psf = nullptr;
				ComputePipelineStateDesc ps_desc;
				ps_desc.cs.code = cs_blob.data();
				ps_desc.cs.length = cs_blob.size();
				lulet(pso, Renderer::device()->new_compute_pipline_state(slayout, ps_desc));
				m_mipmapping_slayout = slayout;
				m_mipmapping_pso = pso;
			}
			lucatchret;
			return RV();
		}

		void TextureImporterType::generate_mipmaps(Gfx::IResource* resource_with_most_detailed_mip, Gfx::ICommandBuffer* compute_cmdbuf)
		{
			using namespace Gfx;
			auto desc = resource_with_most_detailed_mip->desc();
			lucheck(desc.mip_levels);
			lucheck(desc.type == EResourceType::texture_2d);
			lucheck(desc.depth_or_array_size == 1);

			if (desc.mip_levels == 1)
			{
				return;
			}

			compute_cmdbuf->set_compute_shader_input_layout(m_mipmapping_slayout);
			compute_cmdbuf->set_pipeline_state(m_mipmapping_pso);
			u32 cb_align;
			Renderer::device()->check_feature_support(EDeviceFeature::buffer_data_alignment, &cb_align);
			u32 cb_size = (u32)align_upper(sizeof(Float2), cb_align);
			auto cb = Renderer::device()->new_resource(
				ResourceDesc::buffer(EAccessType::upload, EResourceUsageFlag::constant_buffer, cb_size * (desc.mip_levels - 1))).get();

			void* mapped = cb->map_subresource(0, false, 1, 0).get();
			for (u32 j = 0; j < (u32)(desc.mip_levels - 1); ++j)
			{
				u32 width = max<u32>((u32)desc.width >> (j + 1), 1);
				u32 height = max<u32>(desc.height >> (j + 1), 1);
				Float2U* dest = (Float2U*)((usize)mapped + cb_size * j);
				dest->x = 1.0f / (f32)width;
				dest->y = 1.0f / (f32)height;
			}
			cb->unmap_subresource(0, true);

			u32 width = (u32)desc.width / 2;
			u32 height = desc.height / 2;

			for (u32 j = 0; j < (u32)(desc.mip_levels - 1); ++j)
			{
				u32 src_subresource = Gfx::calc_subresource_index(j, 0, desc.mip_levels);
				u32 dest_subresource = Gfx::calc_subresource_index(j + 1, 0, desc.mip_levels);
				ResourceBarrierDesc barriers[] = {
					ResourceBarrierDesc::as_transition(resource_with_most_detailed_mip, EResourceState::shader_resource_non_pixel, src_subresource),
					ResourceBarrierDesc::as_transition(resource_with_most_detailed_mip, EResourceState::unordered_access, dest_subresource)
				};
				compute_cmdbuf->resource_barriers(2, barriers);
				auto vs = Renderer::device()->new_view_set(m_mipmapping_slayout, ViewSetDesc(1, 1, 1, 1)).get();
				vs->set_cbv(0, cb, ConstantBufferViewDesc(cb_size * j, cb_size));
				vs->set_srv(0, resource_with_most_detailed_mip, &ShaderResourceViewDesc::as_tex2d(desc.format, src_subresource, 1, 0.0f));
				vs->set_uav(0, resource_with_most_detailed_mip, nullptr, &UnorderedAccessViewDesc::as_tex2d(desc.format, dest_subresource));
				vs->set_sampler(0, SamplerDesc(EFilter::min_mag_mip_linear, ETextureAddressMode::warp, ETextureAddressMode::warp, ETextureAddressMode::warp));
				compute_cmdbuf->set_compute_view_set(vs);
				compute_cmdbuf->attach_graphic_object(vs);
				compute_cmdbuf->dispatch(max<u32>(width / 8, 1), max<u32>(height / 8, 1), 1);
				width = max<u32>(width / 2, 1);
				height = max<u32>(height / 2, 1);
			}

			compute_cmdbuf->resource_barrier(ResourceBarrierDesc::as_transition(resource_with_most_detailed_mip, EResourceState::common));

			luassert_always(succeeded(compute_cmdbuf->submit()));
			compute_cmdbuf->wait();
			luassert_always(succeeded(compute_cmdbuf->reset()));
		}

		P<IAssetEditor> Luna::editor::TextureImporterType::new_importer(const Path& create_dir)
		{
			auto dialog = newobj<TextureImporter>();
			dialog->m_create_dir = create_dir;
			dialog->m_type = this;
			return dialog;
		}

		void TextureImporter::on_render(ImGui::IContext* ctx)
		{
			char title[32];
			sprintf_s(title, "Texture Importer###%d", (u32)(usize)this);

			ctx->begin(title, &m_open, ImGui::EWindowFlag::no_collapse);

			if (ctx->button("Select Source File"))
			{
				lutry
				{
					lulet(img_path, Gfx::open_file_dialog("Image File\0*.jpg;*.jpeg;*.png;*.png;*.tga;*.bmp;*.psd;*.gif;*.hdr;*.pic\0\0", 
						"Select Source File"));
					// Open file.
					lulet(img_file, platform_open_file(img_path[0].encode(EPathSeparator::system_preferred).c_str(), 
						EFileOpenFlag::read | EFileOpenFlag::user_buffering, EFileCreationMode::open_existing));
					luset(m_desc, Image::load_image_desc(img_file));
					m_source_file_path = img_path[0];
				}
				lucatch
				{
					if (lures != BasicError::user_canceled())
					{
						auto _ = Gfx::message_box(get_errmsg(lures), "Failed to import texture",
							Gfx::EMessageBoxType::ok, Gfx::EMessageBoxIcon::error);
					}
					m_source_file_path = nullptr;
				}
			}

			if (m_source_file_path.empty())
			{
				ctx->text("No image file selected.");
			}
			else
			{
				ctx->text(m_source_file_path.encode().c_str());
				ctx->text("Texture Information:");
				ctx->text("Width: %u", m_desc.width);
				ctx->text("Height: %u", m_desc.height);
				const char* fmt = nullptr;
				switch (m_desc.format)
				{
				case Image::EImagePixelFormat::r8_unorm:
					fmt = "R8 UNORM"; break;
				case Image::EImagePixelFormat::r16_unorm:
					fmt = "R16 UNORM"; break;
				case Image::EImagePixelFormat::r32_float:
					fmt = "R32 FLOAT"; break;
				case Image::EImagePixelFormat::rg8_unorm:
					fmt = "RG8 UNORM"; break;
				case Image::EImagePixelFormat::rg16_unorm:
					fmt = "RG16 UNORM"; break;
				case Image::EImagePixelFormat::rg32_float:
					fmt = "RG32 FLOAT"; break;
				case Image::EImagePixelFormat::rgb8_unorm:
					fmt = "RGB8 UNORM"; break;
				case Image::EImagePixelFormat::rgb16_unorm:
					fmt = "RGB16 UNORM"; break;
				case Image::EImagePixelFormat::rgb32_float:
					fmt = "RGB32 FLOAT"; break;
				case Image::EImagePixelFormat::rgba8_unorm:
					fmt = "RGBA8 UNORM"; break;
				case Image::EImagePixelFormat::rgba16_unorm:
					fmt = "RGBA16 UNORM"; break;
				case Image::EImagePixelFormat::rgba32_float:
					fmt = "RGBA32 FLOAT"; break;
				default:
					lupanic();
					break;
				}
				ctx->text("Format: %s", fmt);

				ctx->text("Import Settings:");

				const char* items[] = { "R8 UNORM", "R16 UNORM", "R32 FLOAT", "RG8 UNORM", "RG16 UNORM", "RG32 FLOAT", "RGB32 FLOAT",
				"RGBA8 UNORM", "RGBA16 UNORM", "RGBA32 FLOAT" };
				ctx->combo("Import Format", &m_import_format, items, 10);
				ctx->checkbox("Used as Render Target", &m_allow_render_target);
				ctx->input_text("Asset Name", m_asset_name);
				if (!m_asset_name.empty())
				{
					ctx->text("The texture will be imported as: %s%s", m_create_dir.encode().c_str(), m_asset_name.c_str());
					if (ctx->button("Import"))
					{
						// Import logic.
						Image::EImagePixelFormat import_format;
						switch (m_import_format)
						{
						case 0: import_format = Image::EImagePixelFormat::r8_unorm; break;
						case 1: import_format = Image::EImagePixelFormat::r16_unorm; break;
						case 2: import_format = Image::EImagePixelFormat::r32_float; break;
						case 3: import_format = Image::EImagePixelFormat::rg8_unorm; break;
						case 4: import_format = Image::EImagePixelFormat::rg16_unorm; break;
						case 5: import_format = Image::EImagePixelFormat::rg32_float; break;
						case 6: import_format = Image::EImagePixelFormat::rgb32_float; break;
						case 7: import_format = Image::EImagePixelFormat::rgba8_unorm; break;
						case 8: import_format = Image::EImagePixelFormat::rgba16_unorm; break;
						case 9: import_format = Image::EImagePixelFormat::rgba32_float; break;
						default: lupanic(); break;
						}

						using namespace Gfx;

						lutry2
						{
							lulet2(imgf, platform_open_file(m_source_file_path.encode(EPathSeparator::system_preferred).c_str(), 
								EFileOpenFlag::read | EFileOpenFlag::user_buffering, EFileCreationMode::open_existing));
							lulet2(img, Image::load_image(imgf, &import_format));
							lulet2(img_asset, Texture::new_texture());

							EResourceFormat format;

							switch (import_format)
							{
							case Image::EImagePixelFormat::r8_unorm: format = EResourceFormat::r8_unorm; break;
							case Image::EImagePixelFormat::r16_unorm: format = EResourceFormat::r16_unorm; break;
							case Image::EImagePixelFormat::r32_float: format = EResourceFormat::r32_float; break;
							case Image::EImagePixelFormat::rg8_unorm: format = EResourceFormat::rg8_unorm; break;
							case Image::EImagePixelFormat::rg16_unorm: format = EResourceFormat::rg16_unorm; break;
							case Image::EImagePixelFormat::rg32_float: format = EResourceFormat::rg32_float; break;
							case Image::EImagePixelFormat::rgb32_float: format = EResourceFormat::rgb32_float; break;
							case Image::EImagePixelFormat::rgba8_unorm: format = EResourceFormat::rgba8_unorm; break;
							case Image::EImagePixelFormat::rgba16_unorm: format = EResourceFormat::rgba16_unorm; break;
							case Image::EImagePixelFormat::rgba32_float: format = EResourceFormat::rgba32_float; break;
							}

							EResourceUsageFlag flags = EResourceUsageFlag::shader_resource | EResourceUsageFlag::unordered_access;
							if (m_allow_render_target)
							{
								flags |= EResourceUsageFlag::render_target;
							}

							ResourceDesc desc = ResourceDesc::tex2d(format, EAccessType::gpu_local, flags, m_desc.width, m_desc.height, 1);

							Texture::SubresourceData initial_data;
							initial_data.data = img->data();
							initial_data.depth = 1;
							initial_data.width = m_desc.width;
							initial_data.height = m_desc.height;
							initial_data.format = format;
							initial_data.row_pitch = m_desc.width * (u32)bits_per_pixel(format) / 8;
							initial_data.subresource = 0;

							img_asset->reset(desc, &initial_data, 1);

							// Wait until the asset is prepared.
							while (img_asset->meta()->state() == Asset::EAssetState::loading)
							{
								yield_current_thread();
							}

							// Throw if the asset initialization failed.
							if ((img_asset->meta()->flags() & Asset::EAssetFlag::loading_error) != Asset::EAssetFlag::none)
							{
								auto err_obj = img_asset->meta()->error_object();
								get_error_object() = err_obj;
								luthrow2(BasicError::error_object());
							}

							// Save the asset.
							auto ass_path = m_create_dir;
							ass_path.push_back(Name(m_asset_name.c_str()));
							ass_path.flags() = (ass_path.flags() & ~EPathFlag::diretory);
							
							luexp2(img_asset->meta()->set_meta_path(ass_path));
							img_asset->meta()->set_data_path(ass_path);

							// Generate mipmaps.
							auto tex = img_asset->texture().get();
							auto cmdbuf = Renderer::main_compute_queue()->new_command_buffer().get();
							m_type->generate_mipmaps(tex, cmdbuf);

							lulet2(r1, img_asset->meta()->save_meta(Asset::EAssetSaveFormat::ascii));
							lulet2(r2, img_asset->meta()->save_data(Asset::EAssetSaveFormat::ascii));
							r1->wait();
							r2->wait();

						}
						lucatch2
						{
							auto _ = Gfx::message_box(get_errmsg(lures2), "Failed to create texture asset",
							Gfx::EMessageBoxType::ok, Gfx::EMessageBoxIcon::error);
						}
					}
				}
			}

			ctx->end();
		}
	}
}