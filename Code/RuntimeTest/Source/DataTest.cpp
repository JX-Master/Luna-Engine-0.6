// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file HashTest.cpp
* @author JXMaster
* @date 2020/2/19
*/
#include "TestCommon.hpp"

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

namespace luna
{
	static void serialize()
	{
		auto table = new_var(EVariantType::table);

		auto pos = new_var1(EVariantType::f32, 3);
		pos->f32_buf()[0] = 0.0f;
		pos->f32_buf()[1] = 10.0f;
		pos->f32_buf()[2] = 50.0f;
		table->set_field(0, intern_name("pos"), pos);

		auto mana = new_var(EVariantType::u16);
		mana->u16() = 150;
		table->set_field(0, intern_name("mana"), mana);

		auto hp = new_var(EVariantType::u16);
		hp->u16() = 100;
		table->set_field(0, intern_name("hp"), hp);

		auto name = new_var(EVariantType::string);
		name->str() = new_string("Baby Gragon");
		table->set_field(0, intern_name("name"), name);

		auto friendly = new_var(EVariantType::boolean);
		bit_reset(friendly->boolean_buf(), 0);
		table->set_field(0, intern_name("friendly"), friendly);

		auto equipment = new_var1(EVariantType::string, 3);
		equipment->str_buf()[0] = new_string("normal shield");
		equipment->str_buf()[1] = new_string("leather shoes");
		equipment->str_buf()[2] = new_string("diamond ring");
		table->set_field(0, intern_name("equipment"), equipment);

		auto default_tex = new_var2(EVariantType::f32, 2, 2);
		float32 tex_data[4] = { 0.0f, 1.0f, 1.0f, 0.0f };
		memcpy(default_tex->f32_buf(), tex_data, sizeof(float32) * 4);

		auto shader_param = new_var(EVariantType::table);
		{
			auto type = new_var(EVariantType::string);
			type->str() = new_string("opaque");
			shader_param->set_field(0, intern_name("type"), type);
			auto col = new_var1(EVariantType::f32, 4);
			float32 color_data[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
			memcpy(col->f32_buf(), color_data, sizeof(float32) * 4);
			shader_param->set_field(0, intern_name("color"), col);
		}
		table->set_field(0, intern_name("shader_param"), shader_param);

		auto material = new_var1(EVariantType::table, 2);
		{
			auto tex = new_var(EVariantType::path);
			tex->path() = new_path("./tex/Tex1.dds");
			material->set_field(0, intern_name("tex"), tex);
			auto name = new_var(EVariantType::name);
			name->name() = intern_name("Mat.001");
			material->set_field(0, intern_name("name"), name);

			tex = new_var(EVariantType::path);
			tex->path() = new_path("./tex/Tex2.dds");
			material->set_field(1, intern_name("tex"), tex);
			name = new_var(EVariantType::name);
			name->name() = intern_name("Mat.002");
			material->set_field(1, intern_name("name"), name);
		}
		table->set_field(0, intern_name("material"), material);

		auto userdata = new_var1(EVariantType::variant, 3);
		{
			auto s = new_var(EVariantType::string);
			s->str() = new_string("to be finished.");
			userdata->var_buf()[0] = s;
			auto s2 = new_var2(EVariantType::u16, 4, 3);
			uint16 data[12] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
			memcpy(s2->u16_buf(), data, sizeof(uint16) * 12);
			userdata->var_buf()[1] = s2;
			auto s3 = new_var(EVariantType::null);
			userdata->var_buf()[2] = s3;
		}
		table->set_field(0, intern_name("userdata"), userdata);

		auto encoder = new_text_encoder();
		auto file = platform_open_file("./Monster.la", EFileOpenFlag::write | EFileOpenFlag::user_buffering, EFileCreationMode::create_always).get();
		lutest(succeeded(encoder->encode(table, file)));
		file = nullptr;
	}

	static void deserialize()
	{
		auto file = platform_open_file("./Monster.la", EFileOpenFlag::read | EFileOpenFlag::user_buffering, EFileCreationMode::open_existing).get();
		auto decoder = new_text_decoder();
		auto var = decoder->decode(file).get();
		file = nullptr;
		platform_delete_file("./Monster.la");

		auto pos = var->field(0, intern_name("pos")).get();
		lutest(pos->f32_buf()[0] == 0.0f);
		lutest(pos->f32_buf()[1] == 10.0f);
		lutest(pos->f32_buf()[2] == 50.0f);

		auto mana = var->field(0, intern_name("mana")).get();
		lutest(mana->u16_buf()[0] == 150);
		auto hp = var->field(0, intern_name("hp")).get();
		lutest(hp->u16_buf()[0] == 100);

		auto name = var->field(0, intern_name("name")).get();
		lutest(!strcmp(name->str()->c_str(), "Baby Gragon"));

		auto friendly = var->field(0, intern_name("friendly")).get();
		lutest(bit_test(friendly->boolean_buf(), 0) == false);

		auto equipment = var->field(0, intern_name("equipment")).get();
		lutest(!strcmp(equipment->str_buf()[0]->c_str(), "normal shield"));
		lutest(!strcmp(equipment->str_buf()[1]->c_str(), "leather shoes"));
		lutest(!strcmp(equipment->str_buf()[2]->c_str(), "diamond ring"));

		auto shader_param = var->field(0, intern_name("shader_param")).get();
		auto shader_param_type = shader_param->field(0, intern_name("type")).get();
		lutest(!strcmp(shader_param_type->str_buf()[0]->c_str(), "opaque"));
		auto shader_param_color = shader_param->field(0, intern_name("color")).get();
		lutest(shader_param_color->f32_buf()[0] == 1.0f);
		lutest(shader_param_color->f32_buf()[1] == 1.0f);
		lutest(shader_param_color->f32_buf()[2] == 1.0f);
		lutest(shader_param_color->f32_buf()[3] == 1.0f);

		auto material = var->field(0, intern_name("material")).get();
		auto material_tex = material->field(0, intern_name("tex")).get();
		auto material_name = material->field(0, intern_name("name")).get();
		auto path1 = new_path("./tex/Tex1.dds");
		lutest(material_tex->path()->equal_to(path1));
		lutest(!strcmp(material_name->name()->c_str(), "Mat.001"));
		material_tex = material->field(1, intern_name("tex")).get();
		material_name = material->field(1, intern_name("name")).get();
		auto path2 = new_path("./tex/Tex2.dds");
		lutest(material_tex->path()->equal_to(path2));
		lutest(material_name->name() == intern_name("Mat.002"));

		auto userdata = var->field(0, intern_name("userdata")).get();
		auto userdata_0 = userdata->var_buf()[0];
		auto userdata_1 = userdata->var_buf()[1];
		auto userdata_2 = userdata->var_buf()[2];
		lutest(!strcmp(userdata_0->str()->c_str(), "to be finished."));
		lutest(userdata_1->length(1) == 4);
		lutest(userdata_1->length(2) == 3);
		lutest(userdata_1->u16_buf()[0] == 1);
		lutest(userdata_1->u16_buf()[1] == 2);
		lutest(userdata_1->u16_buf()[2] == 3);
		lutest(userdata_1->u16_buf()[3] == 4);
		lutest(userdata_1->u16_buf()[4] == 5);
		lutest(userdata_1->u16_buf()[5] == 6);
		lutest(userdata_1->u16_buf()[6] == 7);
		lutest(userdata_1->u16_buf()[7] == 8);
		lutest(userdata_1->u16_buf()[8] == 9);
		lutest(userdata_1->u16_buf()[9] == 10);
		lutest(userdata_1->u16_buf()[10] == 11);
		lutest(userdata_1->u16_buf()[11] == 12);
		lutest(userdata_2->type() == EVariantType::null);
	}

	void data_test()
	{
		serialize();
		deserialize();
	}
}