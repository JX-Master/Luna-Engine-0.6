// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file MathCore.hpp
* @author JXMaster
* @date 2019/1/5
 */
#pragma once
#include <Base/BaseDefines.hpp>

namespace luna
{
	namespace input
	{
		//! A platform-independent key code mapping for standard QWERTY keyboard.
		enum class EKeyCode : uint32
		{
			invalid = 0x00,
			backspace = 0x08,
			tab = 0x09,
			clear = 0x0C,
			enter = 0x0D,
			shift = 0x10,
			ctrl = 0x11,
			alt = 0x12,
			pause = 0x13,
			capsLock = 0x14,
			esc = 0x1B,
			space_bar = 0x20,
			page_up = 0x21,
			page_down = 0x22,
			end = 0x23,
			home = 0x24,
			left = 0x25,
			up = 0x26,
			right = 0x27,
			down = 0x28,
			select = 0x29,
			print = 0x2A,
			execute = 0x2B,
			print_screen = 0x2C,
			insert = 0x2D,
			del = 0x2E,
			help = 0x2F,
			zero = 0x30,
			one = 0x31,
			two = 0x32,
			three = 0x33,
			four = 0x34,
			five = 0x35,
			six = 0x36,
			seven = 0x37,
			eight = 0x38,
			nine = 0x39,
			a = 0x41,
			b = 0x42,
			c = 0x43,
			d = 0x44,
			e = 0x45,
			f = 0x46,
			g = 0x47,
			h = 0x48,
			i = 0x49,
			j = 0x4A,
			k = 0x4B,
			l = 0x4C,
			m = 0x4D,
			n = 0x4E,
			o = 0x4F,
			p = 0x50,
			q = 0x51,
			r = 0x52,
			s = 0x53,
			t = 0x54,
			u = 0x55,
			v = 0x56,
			w = 0x57,
			x = 0x58,
			y = 0x59,
			z = 0x5A,
			l_system = 0x5B,
			r_system = 0x5C,
			app = 0x5D,
			sleep = 0x5F,
			numpad0 = 0x60,
			numpad1 = 0x61,
			numpad2 = 0x62,
			numpad3 = 0x63,
			numpad4 = 0x64,
			numpad5 = 0x65,
			numpad6 = 0x66,
			numpad7 = 0x67,
			numpad8 = 0x68,
			numpad9 = 0x69,
			multiply = 0x6A,
			add = 0x6B,
			separator = 0x6C,
			subtract = 0x6D,
			decimal = 0x6E,
			divide = 0x6F,
			f1 = 0x70,
			f2 = 0x71,
			f3 = 0x72,
			f4 = 0x73,
			f5 = 0x74,
			f6 = 0x75,
			f7 = 0x76,
			f8 = 0x77,
			f9 = 0x78,
			f10 = 0x79,
			f11 = 0x7A,
			f12 = 0x7B,
			f13 = 0x7C,
			f14 = 0x7D,
			f15 = 0x7E,
			f16 = 0x7F,
			f17 = 0x80,
			f18 = 0x81,
			f19 = 0x82,
			f20 = 0x83,
			f21 = 0x84,
			f22 = 0x85,
			f23 = 0x86,
			f24 = 0x87,
			num_lock = 0x90,
			scroll_lock = 0x91,
			l_shift = 0xA0,
			r_shift = 0xA1,
			l_ctrl = 0xA2,
			r_ctrl = 0xA3,
			l_alt = 0xA4,
			r_alt = 0xA5,
			semicolon = 0xBA,	// ;: key on US standard keyboard
			plus = 0xBB,		// =+ key
			comma = 0xBC,		// ,< key
			minus = 0xBD,		// -_ key
			dot = 0xBE,			// .> key
			slash = 0xBF,		// /? key on US standard keyboard
			wave = 0xC0,		// ~` key on US standard keyboard
			l_branket = 0xDB,	// [{ key on US standard keyboard
			backslash = 0xDC,	// \| key on US standard keyboard
			r_branket = 0xDD,	// ]} key on US standard keyboard
			quote = 0xDE,		// '" key on US standard keyboard
		};

		enum class EMouseKey : uint32
		{
			none = 0x00,
			lb = 0x01,
			rb = 0x02,
			mb = 0x04,
			x1b = 0x08,
			x2b = 0x10,
		};

		enum class EControllerAxis : uint32
		{
			lx = 0,	// Left thumb pad X axis.
			ly = 1,	// Left thumb pad Y axis.
			rx = 2,	// Right thumb pad X axis.
			ry = 3,	// Right thumb pad Y axis.
			lt = 4,	// Left trigger.
			rt = 5,	// Right trigger.
		};

		enum class EControllerButton : uint32
		{
			lthumb = 0,	// Left thumb button.
			rthumb = 1,	// Right thumb button.
			lup = 2,	// Left up button.
			ldown = 3,	// Left down button.
			lleft = 4,	// Left left button.
			lright = 5,	// Left right button.
			rup = 6,	// Right up button.
			rdown = 7,	// Right down button.
			rleft = 8,	// Right left button.
			rright = 9,	// Right right button.
			lb = 10,	// Left shoulder button.
			rb = 11,	// Right shoulder button.
			lt = 12,	// Left trigger button.
			rt = 13,	// Right trigger button.
			lspecial = 14,	// Left special button.
			rspecial = 15	// Right special button.
		};

		//! Identify the vibration motor to set in `set_vibration`.
		enum class EControllerVibrationMotor : uint32
		{
			left = 1,
			right = 2,
		};
	}
}