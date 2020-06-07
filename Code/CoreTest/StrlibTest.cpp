// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file StrlibTest.cpp
* @author JXMaster
* @date 2020/2/20
*/
#include "TestCommon.hpp"

namespace luna
{
	void strlib_test()
	{
		{
			// name object test.
			auto name1 = intern_name("Thomas");
			auto name2 = intern_name("Jack");
			auto name3 = intern_name("Thomas");
			auto name4 = intern_name("Thomas", 3);
			lutest(name1 != name2);
			lutest(name1 == name3);
			lutest(name1 != name4);

			// name string ends with \0.
			lutest(name1->c_str()[6] == 0);
			lutest(name2->c_str()[4] == 0);
			lutest(name3->c_str()[6] == 0);
			lutest(name4->c_str()[3] == 0);

			// generated names.
			char str[16];
			P<IName> names[100];
			for (int i = 0; i < 50; ++i)
			{
				sprintf_s(str, "Name%d", i);
				names[i] = intern_name(str);
			}
			for (int i = 0; i < 50; ++i)
			{
				sprintf_s(str, "Name%d", i);
				names[50 + i] = intern_name(str);
			}
			for (int i = 0; i < 50; ++i)
			{
				lutest(names[i] == names[50 + i]);
			}
		}

		{
			// Path object.
			const char* s1 = "C:/Sample Dir/Sample File.txt";
			const char* s2 = "/Sample Dir/Sample File.txt";
			const char* s3 = "Sample Dir/Sample Dir2/";	// path detects directory if the path string ends with `/`.

			// create and compare.
			auto p1 = new_path();
			p1->parse(s1);
			auto p2 = new_path();
			p2->parse(s1);
			auto p3 = new_path();
			p3->parse(s2);
			auto p4 = new_path();
			p4->parse(s3);
			lutest(p1->equal_to(p2));
			lutest(!p1->equal_to(p3));

			// assign().
			auto p5 = new_path();
			p5->assign(p1);
			lutest(p5->equal_to(p1));

			// parse()
			p2->parse(s2);
			lutest(!p1->equal_to(p2));
			lutest(p2->equal_to(p3));
			lutest(p1->hash_code() != p2->hash_code());
			lutest(p2->hash_code() == p3->hash_code());

			// iterate.
			lutest(p1->count_nodes() == 2);
			lutest(p2->count_nodes() == 2);
			lutest(p3->count_nodes() == 2);
			lutest(p1->node(0) == intern_name("Sample Dir"));
			lutest(p1->node(1) == intern_name("Sample File.txt"));
			lutest(p3->node(0) == intern_name("Sample Dir"));
			lutest(p3->node(1) == intern_name("Sample File.txt"));

			// attributes.
			lutest((p1->flags() & EPathFlag::absolute) != EPathFlag::none);
			lutest((p3->flags() & EPathFlag::absolute) != EPathFlag::none);
			lutest((p4->flags() & EPathFlag::absolute) == EPathFlag::none);

			lutest((p1->flags() & EPathFlag::diretory) == EPathFlag::none);
			lutest((p3->flags() & EPathFlag::diretory) == EPathFlag::none);
			lutest((p4->flags() & EPathFlag::diretory) != EPathFlag::none);

			lutest(p1->root_name() == intern_name("C:"));
			lutest(!p3->root_name());
			lutest(!p4->root_name());

			// encode.
			auto str1 = p1->encode();
			auto str2 = p3->encode();
			auto str3 = p4->encode();
			lutest(!strcmp(str1->c_str(), s1));
			lutest(!strcmp(str2->c_str(), s2));
			lutest(!strcmp(str3->c_str(), s3));
		}

		{
			// Path object.
			// extension
			const char* s1 = "C:/Sample Dir/Sample File.txt";
			const char* s2 = "/Sample Dir/Sample File.TXT";
			auto p1 = new_path();
			p1->parse(s1);
			auto p2 = new_path();
			p2->parse(s2);
			lutest(!p1->equal_to(p2));
			P<IName> ext1 = p1->extension();
			P<IName> ext2 = p2->extension();
			lutest(ext1 == ext2);
			lutest(ext1 == intern_name("txt"));
		}

		{
			// Path object.
			// is_subpath_of
			// relative_to

			const char* s1 = "C:/Sample Dir/Sample File.txt";
			const char* s2 = "D:/Sample Dir/Sample File.txt";
			const char* s3 = "C:/Sample Dir/";
			const char* s4 = "Sample Dir";
			const char* s5 = "C:/Sample Dir/Same Dir2/";
			auto p1 = new_path();
			auto p2 = new_path();
			auto p3 = new_path();
			auto p4 = new_path();
			auto p5 = new_path();
			p1->parse(s1);
			p2->parse(s2);
			p3->parse(s3);
			p4->parse(s4);
			p5->parse(s5);


			lutest(p1->is_subpath_of(p3));
			lutest(p1->is_subpath_of(p4));
			lutest(!p2->is_subpath_of(p3));
			lutest(p2->is_subpath_of(p4));

			auto r1 = new_path();
			auto r2 = new_path();
			auto r3 = new_path();
			r1->assign_relative(p1, p3);	// ../
			r2->assign_relative(p3, p1);	// Sample File.txt
			r3->assign_relative(p3, p5);	// Same Dir2/

			auto rs1 = r1->encode();
			auto rs2 = r2->encode();
			auto rs3 = r3->encode();
			lutest(!strcmp(rs1->c_str(), "C:../"));
			lutest(!strcmp(rs2->c_str(), "C:Sample File.txt"));
			lutest(!strcmp(rs3->c_str(), "C:Same Dir2/"));
		}
	}
}