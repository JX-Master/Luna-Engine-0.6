// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file PathTest.cpp
* @author JXMaster
* @date 2020/8/16
*/
#include "TestCommon.hpp"
#include <Runtime/Path.hpp>

namespace Luna
{
	void path_test()
	{
		{
			// Path object.
			const char* s1 = "C:/Sample Dir/Sample File.txt";
			const char* s2 = "/Sample Dir/Sample File.txt";
			const char* s3 = "Sample Dir/Sample Dir2/";	// path detects directory if the path string ends with `/`.

			// create and compare.
			Path p1(s1);
			Path p2(s1);
			Path p3(s2);
			Path p4(s3);
			lutest(p1 == p2);
			lutest(p1 != p3);

			// assign().
			Path p5;
			p5.assign(p1);
			lutest(p5 == p1);

			// assign() with strings.
			p2.assign(s2);
			lutest(p1 != p2);
			lutest(p2 == p3);
			lutest(p1.hash_code() != p2.hash_code());
			lutest(p2.hash_code() == p3.hash_code());

			// iterate.
			lutest(p1.size() == 2);
			lutest(p2.size() == 2);
			lutest(p3.size() == 2);
			lutest(p1[0] == Name("Sample Dir"));
			lutest(p1[1] == Name("Sample File.txt"));
			lutest(p3[0] == Name("Sample Dir"));
			lutest(p3[1] == Name("Sample File.txt"));

			// attributes.
			lutest((p1.flags() & EPathFlag::absolute) != EPathFlag::none);
			lutest((p3.flags() & EPathFlag::absolute) != EPathFlag::none);
			lutest((p4.flags() & EPathFlag::absolute) == EPathFlag::none);

			lutest((p1.flags() & EPathFlag::diretory) == EPathFlag::none);
			lutest((p3.flags() & EPathFlag::diretory) == EPathFlag::none);
			lutest((p4.flags() & EPathFlag::diretory) != EPathFlag::none);

			lutest(p1.root() == Name("C:"));
			lutest(!p3.root());
			lutest(!p4.root());

			// encode.
			auto str1 = p1.encode();
			auto str2 = p3.encode();
			auto str3 = p4.encode();
			lutest(!strcmp(str1.c_str(), s1));
			lutest(!strcmp(str2.c_str(), s2));
			lutest(!strcmp(str3.c_str(), s3));
		}
		

		{
			// Path object.
			// extension
			const char* s1 = "C:/Sample Dir/Sample File.txt";
			const char* s2 = "/Sample Dir/Sample File.TXT";
			Path p1(s1);
			Path p2(s2);
			lutest(p1 != p2);
			Name ext1 = p1.extension();
			Name ext2 = p2.extension();
			lutest(ext1 == ext2);
			lutest(ext1 == Name("txt"));
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
			Path p1(s1);
			Path p2(s2);
			Path p3(s3);
			Path p4(s4);
			Path p5(s5);

			lutest(p1.is_subpath_of(p3));
			lutest(p1.is_subpath_of(p4));
			lutest(!p2.is_subpath_of(p3));
			lutest(p2.is_subpath_of(p4));

			Path r1;
			Path r2;
			Path r3;
			r1.assign_relative(p1, p3);	// ../
			r2.assign_relative(p3, p1);	// Sample File.txt
			r3.assign_relative(p3, p5);	// Same Dir2/

			auto rs1 = r1.encode();
			auto rs2 = r2.encode();
			auto rs3 = r3.encode();
			lutest(!strcmp(rs1.c_str(), "C:../"));
			lutest(!strcmp(rs2.c_str(), "C:Sample File.txt"));
			lutest(!strcmp(rs3.c_str(), "C:Same Dir2/"));
		}
	}
}