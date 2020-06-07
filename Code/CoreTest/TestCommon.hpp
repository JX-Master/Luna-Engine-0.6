// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file TestCommon.hpp
* @author JXMaster
* @date 2020/2/16
*/
#pragma once
#include <Core/Core.hpp>

namespace luna
{
	void vector_test();
	void hash_test();
	void strlib_test();
	void vfs_test();
	void ring_deque_test();
	void data_test();

	// STL test framework modified from EASTL.

	constexpr uint32 magic_value_v = 0x01f1cbe8;

	struct TestObject
	{
		int32         m_value;              // Value for the TestObject.
		uint32        m_magic;	            // Used to verify that an instance is valid and that it is not corrupted. It should always be kMagicValue.
		int64         m_id;                 // Unique id for each object, equal to its creation number. This value is not coped from other TestObjects during any operations, including moves.
		bool          m_throw_on_copy;      // Throw an exception of this object is copied, moved, or assigned to another.
		static int64  g_count;		        // Count of all current existing TestObjects.
		static int64  g_ctor_count;         // Count of times any ctor was called.
		static int64  g_dtor_count;		    // Count of times dtor was called.
		static int64  g_default_ctor_count; // Count of times the default ctor was called.
		static int64  g_arg_ctor_count;     // Count of times the x0,x1,x2 ctor was called.
		static int64  g_copy_ctor_count;    // Count of times copy ctor was called.
		static int64  g_move_ctor_count;    // Count of times move ctor was called.
		static int64  g_copy_assign_count;  // Count of times copy assignment was called.
		static int64  g_move_assign_count;  // Count of times move assignment was called.
		static int32  g_magic_error_count;  // Number of magic number mismatch errors.

		explicit TestObject(int32 x = 0, bool throw_on_copy = false)
			: m_value(x), m_throw_on_copy(throw_on_copy), m_magic(magic_value_v)
		{
			++g_count;
			++g_ctor_count;
			++g_default_ctor_count;
			m_id = g_ctor_count;
		}

		// This constructor exists for the purpose of testing variadiac template arguments, such as with the emplace container functions.
		TestObject(int32 x0, int32 x1, int32 x2, bool throw_on_copy = false)
			: m_value(x0 + x1 + x2), m_throw_on_copy(throw_on_copy), m_magic(magic_value_v)
		{
			++g_count;
			++g_ctor_count;
			++g_arg_ctor_count;
			m_id = g_ctor_count;
		}

		TestObject(const TestObject& testObject)
			: m_value(testObject.m_value), m_throw_on_copy(testObject.m_throw_on_copy), m_magic(testObject.m_magic)
		{
			++g_count;
			++g_ctor_count;
			++g_copy_ctor_count;
			m_id = g_ctor_count;
			if (m_throw_on_copy)
			{
				lupanic_msg_always("Disallowed TestObject copy");
			}
		}

		// Due to the nature of TestObject, there isn't much special for us to 
		// do in our move constructor. A move constructor swaps its contents with 
		// the other object, whhich is often a default-constructed object.
		TestObject(TestObject&& testObject)
			: m_value(testObject.m_value), m_throw_on_copy(testObject.m_throw_on_copy), m_magic(testObject.m_magic)
		{
			++g_count;
			++g_ctor_count;
			++g_move_ctor_count;
			m_id = g_ctor_count;	// testObject keeps its mId, and we assign ours anew.
			testObject.m_value = 0;   // We are swapping our contents with the TestObject, so give it our "previous" value.
			if (m_throw_on_copy)
			{
				lupanic_msg_always("Disallowed TestObject copy");
			}
		}

		TestObject& operator=(const TestObject& testObject)
		{
			++g_copy_assign_count;

			if (&testObject != this)
			{
				m_value = testObject.m_value;
				// Leave mId alone.
				m_magic = testObject.m_magic;
				m_throw_on_copy = testObject.m_throw_on_copy;
				if (m_throw_on_copy)
				{
					lupanic_msg_always("Disallowed TestObject copy");
				}
			}
			return *this;
		}

		TestObject& operator=(TestObject&& testObject)
		{
			++g_move_assign_count;

			if (&testObject != this)
			{
				swap(m_value, testObject.m_value);
				// Leave mId alone.
				swap(m_magic, testObject.m_magic);
				swap(m_throw_on_copy, testObject.m_throw_on_copy);

				if (m_throw_on_copy)
				{
					lupanic_msg_always("Disallowed TestObject copy");
				}
			}
			return *this;
		}

		~TestObject()
		{
			if (m_magic != magic_value_v)
				++g_magic_error_count;
			m_magic = 0;
			--g_count;
			++g_dtor_count;
		}

		static void reset()
		{
			g_count = 0;
			g_ctor_count = 0;
			g_dtor_count = 0;
			g_default_ctor_count = 0;
			g_arg_ctor_count = 0;
			g_copy_ctor_count = 0;
			g_move_ctor_count = 0;
			g_copy_assign_count = 0;
			g_move_assign_count = 0;
			g_magic_error_count = 0;
		}

		static bool is_clear() // Returns true if there are no existing TestObjects and the sanity checks related to that test OK.
		{
			return (g_count == 0) && (g_dtor_count == g_ctor_count) && (g_magic_error_count == 0);
		}
	};

	inline bool operator==(const TestObject& t1, const TestObject& t2)
	{
		return t1.m_value == t2.m_value;
	}

	inline bool operator<(const TestObject& t1, const TestObject& t2)
	{
		return t1.m_value < t2.m_value;
	}

}

#define lutest luassert_always