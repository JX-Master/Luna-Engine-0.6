// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file HashTest.cpp
* @author JXMaster
* @date 2020/2/19
*/
#include "TestCommon.hpp"
#include <Core/Variant.hpp>

/*
table = {
	pos:f32[3] = { 0.000000, 10.000000, 50.000000 },
	mana:u16 = 150,
	hp:u16 = 100,
	name:string = "Baby Gragon",
	friendly:bool = false,
	equipment:string[3] = { "normal shield", "leather shoes", "diamond ring" },
	shader_param:table = {
			type:string = "opaque",
			color:f32[4] = { 1.000000, 1.000000, 1.000000, 1.000000 }
		},
	material:table[2] = {
		{
			tex:path = "./tex/Tex1.dds",
			name:label = "Mat.001"
		},
		{
			tex:path = "./tex/Tex2.dds",
			name:label = "Mat.002"
		}
	},
	userdata:var[3] = {
		string = "to be finished.",
		u16[3][4] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 },
		null
	}
}
*/

namespace Luna
{
	static void serialize()
	{
		auto table = Variant(EVariantType::table);

		auto pos = Variant(EVariantType::f32, 3);
		pos.to_f32_buf()[0] = 0.0f;
		pos.to_f32_buf()[1] = 10.0f;
		pos.to_f32_buf()[2] = 50.0f;
		table.set_field(0, u8"pos", move(pos));

		auto mana = Variant(EVariantType::u16);
		mana.to_u16() = 150;
		table.set_field(0, u8"mana", move(mana));

		auto hp = Variant(EVariantType::u16);
		hp.to_u16() = 100;
		table.set_field(0, u8"hp", move(hp));

		auto name = Variant(EVariantType::string);
		name.to_str() = u8"Baby Gragon";
		table.set_field(0, u8"name", move(name));

		auto friendly = Variant(EVariantType::boolean);
		bit_reset(friendly.to_boolean_buf(), 0);
		table.set_field(0, u8"friendly", move(friendly));

		auto equipment = Variant(EVariantType::string, 3);
		equipment.to_str_buf()[0] = u8"normal shield";
		equipment.to_str_buf()[1] = u8"leather shoes";
		equipment.to_str_buf()[2] = u8"diamond ring";
		table.set_field(0, u8"equipment", move(equipment));

		auto default_tex = Variant(EVariantType::f32, 2, 2);
		f32 tex_data[4] = { 0.0f, 1.0f, 1.0f, 0.0f };
		memcpy(default_tex.to_f32_buf(), tex_data, sizeof(f32) * 4);

		auto shader_param = Variant(EVariantType::table);
		{
			auto type = Variant(EVariantType::string);
			type.to_str() = u8"opaque";
			shader_param.set_field(0, u8"type", move(type));
			auto col = Variant(EVariantType::f32, 4);
			f32 color_data[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
			memcpy(col.to_f32_buf(), color_data, sizeof(f32) * 4);
			shader_param.set_field(0, u8"color", move(col));
		}
		table.set_field(0, u8"shader_param", move(shader_param));

		auto material = Variant(EVariantType::table, 2);
		{
			auto tex = Variant(EVariantType::path);
			tex.to_path() = u8"./tex/Tex1.dds";
			material.set_field(0, "tex", move(tex));
			auto name = Variant(EVariantType::name);
			name.to_name() = u8"Mat.001";
			material.set_field(0, u8"name", move(name));

			tex = Variant(EVariantType::path);
			tex.to_path() = u8"./tex/Tex2.dds";
			material.set_field(1, "tex", move(tex));
			name = Variant(EVariantType::name);
			name.to_name() = u8"Mat.002";
			material.set_field(1, "name", move(name));
		}
		table.set_field(0, u8"material", move(material));

		auto userdata = Variant(EVariantType::variant, 3);
		{
			auto s = Variant(EVariantType::string);
			s.to_str() = u8"to be finished.";
			userdata.to_var_buf()[0] = move(s);
			auto s2 = Variant(EVariantType::u16, 4, 3);
			u16 data[12] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
			memcpy(s2.to_u16_buf(), data, sizeof(u16) * 12);
			userdata.to_var_buf()[1] = move(s2);
			auto s3 = Variant(EVariantType::null);
			userdata.to_var_buf()[2] = move(s3);
		}
		table.set_field(0, u8"userdata", move(userdata));

		auto encoder = new_text_encoder();
		auto file = platform_open_file(u8"./Monster.la", EFileOpenFlag::write | EFileOpenFlag::user_buffering, EFileCreationMode::create_always).get();
		lutest(succeeded(encoder->encode(table, file)));
		file = nullptr;
	}

	static void deserialize()
	{
		auto file = platform_open_file(u8"./Monster.la", EFileOpenFlag::read | EFileOpenFlag::user_buffering, EFileCreationMode::open_existing).get();
		auto decoder = new_text_decoder();
		auto r = decoder->decode(file);
		auto& var = r.get();
		file = nullptr;
		//platform_delete_file(u8"./Monster.la");

		auto& pos = var.field(0, u8"pos");
		lutest(pos.to_f32_buf()[0] == 0.0f);
		lutest(pos.to_f32_buf()[1] == 10.0f);
		lutest(pos.to_f32_buf()[2] == 50.0f);

		auto& mana = var.field(0, u8"mana");
		lutest(mana.to_u16_buf()[0] == 150);
		auto& hp = var.field(0, "hp");
		lutest(hp.to_u16_buf()[0] == 100);

		auto& name = var.field(0, u8"name");
		lutest(!strcmp(name.to_str().c_str(), u8"Baby Gragon"));

		auto& friendly = var.field(0, u8"friendly");
		lutest(bit_test(friendly.to_boolean_buf(), 0) == false);

		auto& equipment = var.field(0, u8"equipment");
		lutest(!strcmp(equipment.to_str_buf()[0].c_str(), u8"normal shield"));
		lutest(!strcmp(equipment.to_str_buf()[1].c_str(), u8"leather shoes"));
		lutest(!strcmp(equipment.to_str_buf()[2].c_str(), u8"diamond ring"));

		auto& shader_param = var.field(0, u8"shader_param");
		auto& shader_param_type = shader_param.field(0, u8"type");
		lutest(!strcmp(shader_param_type.to_str_buf()[0].c_str(), u8"opaque"));
		auto& shader_param_color = shader_param.field(0, u8"color");
		lutest(shader_param_color.to_f32_buf()[0] == 1.0f);
		lutest(shader_param_color.to_f32_buf()[1] == 1.0f);
		lutest(shader_param_color.to_f32_buf()[2] == 1.0f);
		lutest(shader_param_color.to_f32_buf()[3] == 1.0f);

		auto& material = var.field(0, u8"material");
		auto& material_tex = material.field(0, u8"tex");
		auto& material_name = material.field(0, u8"name");
		auto path1 = Path(u8"./tex/Tex1.dds");
		lutest(material_tex.to_path().equal_to(path1));
		lutest(!strcmp(material_name.to_name().c_str(), u8"Mat.001"));
		material_tex = material.field(1, u8"tex");
		material_name = material.field(1, u8"name");
		auto path2 = Path(u8"./tex/Tex2.dds");
		lutest(material_tex.to_path().equal_to(path2));
		lutest(material_name.to_name() == u8"Mat.002");

		auto& userdata = var.field(0, u8"userdata");
		auto& userdata_0 = userdata.to_var_buf()[0];
		auto& userdata_1 = userdata.to_var_buf()[1];
		auto& userdata_2 = userdata.to_var_buf()[2];
		lutest(!strcmp(userdata_0.to_str().c_str(), u8"to be finished."));
		lutest(userdata_1.length(1) == 4);
		lutest(userdata_1.length(2) == 3);
		lutest(userdata_1.to_u16_buf()[0] == 1);
		lutest(userdata_1.to_u16_buf()[1] == 2);
		lutest(userdata_1.to_u16_buf()[2] == 3);
		lutest(userdata_1.to_u16_buf()[3] == 4);
		lutest(userdata_1.to_u16_buf()[4] == 5);
		lutest(userdata_1.to_u16_buf()[5] == 6);
		lutest(userdata_1.to_u16_buf()[6] == 7);
		lutest(userdata_1.to_u16_buf()[7] == 8);
		lutest(userdata_1.to_u16_buf()[8] == 9);
		lutest(userdata_1.to_u16_buf()[9] == 10);
		lutest(userdata_1.to_u16_buf()[10] == 11);
		lutest(userdata_1.to_u16_buf()[11] == 12);
		lutest(userdata_2.type() == EVariantType::null);
	}

	void data_test()
	{
		serialize();
		deserialize();
	}
}