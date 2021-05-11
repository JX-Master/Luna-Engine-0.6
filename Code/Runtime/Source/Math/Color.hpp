// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Color.hpp
* @author JXMaster
* @date 2019/1/5
*/
#pragma once
#include "Vector.hpp"

namespace Luna
{
	// Regularly used colors.
	// Reference : http://www.w3school.com.cn/tiy/color.asp
	namespace Color
	{
		constexpr Float4 alice_blue{ 0.941176534f, 0.972549081f, 1.000000000f, 1.000000000f };
		constexpr Float4 antique_white { 0.980392218f, 0.921568692f, 0.843137324f, 1.000000000f };
		constexpr Float4 aqua { 0.000000000f, 1.000000000f, 1.000000000f, 1.000000000f };
		constexpr Float4 aquamarine { 0.498039246f, 1.000000000f, 0.831372619f, 1.000000000f };
		constexpr Float4 azure { 0.498039246f, 1.000000000f, 0.831372619f, 1.000000000f };
		constexpr Float4 beige { 0.960784376f, 0.960784376f, 0.862745166f, 1.000000000f };
		constexpr Float4 bisque { 1.000000000f, 0.894117713f, 0.768627524f, 1.000000000f };
		constexpr Float4 black { 0.000000000f, 0.000000000f, 0.000000000f, 1.000000000f };
		constexpr Float4 blanched_almond { 1.000000000f, 0.921568692f, 0.803921640f, 1.000000000f };
		constexpr Float4 blue { 0.000000000f, 0.000000000f, 1.000000000f, 1.000000000f };
		constexpr Float4 blue_violet { 0.541176498f, 0.168627456f, 0.886274576f, 1.000000000f };
		constexpr Float4 brown { 0.647058845f, 0.164705887f, 0.164705887f, 1.000000000f };
		constexpr Float4 burly_wood { 0.870588303f, 0.721568644f, 0.529411793f, 1.000000000f };
		constexpr Float4 cadet_blue { 0.372549027f, 0.619607866f, 0.627451003f, 1.000000000f };
		constexpr Float4 chartreuse { 0.498039246f, 1.000000000f, 0.000000000f, 1.000000000f };
		constexpr Float4 chocolate { 0.823529482f, 0.411764741f, 0.117647067f, 1.000000000f };
		constexpr Float4 coral { 1.000000000f, 0.498039246f, 0.313725501f, 1.000000000f };
		constexpr Float4 cornflower_blue { 0.392156899f, 0.584313750f, 0.929411829f, 1.000000000f };
		constexpr Float4 cornsilk { 1.000000000f, 0.972549081f, 0.862745166f, 1.000000000f };
		constexpr Float4 crimson { 0.862745166f, 0.078431375f, 0.235294133f, 1.000000000f };
		constexpr Float4 cyan { 0.000000000f, 1.000000000f, 1.000000000f, 1.000000000f };
		constexpr Float4 dark_blue { 0.000000000f, 0.000000000f, 0.545098066f, 1.000000000f };
		constexpr Float4 dark_cyan { 0.000000000f, 0.545098066f, 0.545098066f, 1.000000000f };
		constexpr Float4 dark_golden_rod { 0.721568644f, 0.525490224f, 0.043137256f, 1.000000000f };
		constexpr Float4 dark_gray { 0.662745118f, 0.662745118f, 0.662745118f, 1.000000000f };
		constexpr Float4 dark_green { 0.000000000f, 0.392156899f, 0.000000000f, 1.000000000f };
		constexpr Float4 dark_khaki { 0.741176486f, 0.717647076f, 0.419607878f, 1.000000000f };
		constexpr Float4 dark_magenta { 0.545098066f, 0.000000000f, 0.545098066f, 1.000000000f };
		constexpr Float4 dark_olive_green { 0.333333343f, 0.419607878f, 0.184313729f, 1.000000000f };
		constexpr Float4 dark_orange { 1.000000000f, 0.549019635f, 0.000000000f, 1.000000000f };
		constexpr Float4 dark_orchid { 0.600000024f, 0.196078449f, 0.800000072f, 1.000000000f };
		constexpr Float4 dark_red { 0.545098066f, 0.000000000f, 0.000000000f, 1.000000000f };
		constexpr Float4 dark_salmon { 0.913725555f, 0.588235319f, 0.478431404f, 1.000000000f };
		constexpr Float4 dark_sea_green { 0.560784340f, 0.737254918f, 0.545098066f, 1.000000000f };
		constexpr Float4 dark_slate_blue { 0.282352954f, 0.239215702f, 0.545098066f, 1.000000000f };
		constexpr Float4 dark_slate_gray { 0.184313729f, 0.309803933f, 0.309803933f, 1.000000000f };
		constexpr Float4 dark_turquoise { 0.000000000f, 0.807843208f, 0.819607913f, 1.000000000f };
		constexpr Float4 dark_violet { 0.580392182f, 0.000000000f, 0.827451050f, 1.000000000f };
		constexpr Float4 deep_pink { 1.000000000f, 0.078431375f, 0.576470613f, 1.000000000f };
		constexpr Float4 deep_sky_blue { 0.000000000f, 0.749019623f, 1.000000000f, 1.000000000f };
		constexpr Float4 dim_gray { 0.411764741f, 0.411764741f, 0.411764741f, 1.000000000f };
		constexpr Float4 dodger_blue { 0.117647067f, 0.564705908f, 1.000000000f, 1.000000000f };
		constexpr Float4 fire_brick { 0.698039234f, 0.133333340f, 0.133333340f, 1.000000000f };
		constexpr Float4 floral_white { 1.000000000f, 0.980392218f, 0.941176534f, 1.000000000f };
		constexpr Float4 forest_green { 0.133333340f, 0.545098066f, 0.133333340f, 1.000000000f };
		constexpr Float4 fuchsia { 1.000000000f, 0.000000000f, 1.000000000f, 1.000000000f };
		constexpr Float4 gainsboro { 0.862745166f, 0.862745166f, 0.862745166f, 1.000000000f };
		constexpr Float4 ghost_white { 0.972549081f, 0.972549081f, 1.000000000f, 1.000000000f };
		constexpr Float4 gold { 1.000000000f, 0.843137324f, 0.000000000f, 1.000000000f };
		constexpr Float4 golden_rod { 0.854902029f, 0.647058845f, 0.125490203f, 1.000000000f };
		constexpr Float4 gray { 0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f };
		constexpr Float4 green { 0.000000000f, 0.501960814f, 0.000000000f, 1.000000000f };
		constexpr Float4 green_yellow { 0.678431392f, 1.000000000f, 0.184313729f, 1.000000000f };
		constexpr Float4 honey_dew { 0.941176534f, 1.000000000f, 0.941176534f, 1.000000000f };
		constexpr Float4 hot_pink { 1.000000000f, 0.411764741f, 0.705882370f, 1.000000000f };
		constexpr Float4 indian_red { 0.803921640f, 0.360784322f, 0.360784322f, 1.000000000f };
		constexpr Float4 indigo { 0.294117659f, 0.000000000f, 0.509803951f, 1.000000000f };
		constexpr Float4 ivory { 1.000000000f, 1.000000000f, 0.941176534f, 1.000000000f };
		constexpr Float4 khaki { 0.941176534f, 0.901960850f, 0.549019635f, 1.000000000f };
		constexpr Float4 lavender { 0.901960850f, 0.901960850f, 0.980392218f, 1.000000000f };
		constexpr Float4 lavender_blush { 1.000000000f, 0.941176534f, 0.960784376f, 1.000000000f };
		constexpr Float4 lawn_green { 0.486274540f, 0.988235354f, 0.000000000f, 1.000000000f };
		constexpr Float4 lemon_chiffon { 1.000000000f, 0.980392218f, 0.803921640f, 1.000000000f };
		constexpr Float4 light_blue { 0.678431392f, 0.847058892f, 0.901960850f, 1.000000000f };
		constexpr Float4 light_coral { 0.941176534f, 0.501960814f, 0.501960814f, 1.000000000f };
		constexpr Float4 light_cyan { 0.878431439f, 1.000000000f, 1.000000000f, 1.000000000f };
		constexpr Float4 light_golden_rod_yellow { 0.980392218f, 0.980392218f, 0.823529482f, 1.000000000f };
		constexpr Float4 light_gray { 0.827451050f, 0.827451050f, 0.827451050f, 1.000000000f };
		constexpr Float4 light_green { 0.564705908f, 0.933333397f, 0.564705908f, 1.000000000f };
		constexpr Float4 light_pink { 1.000000000f, 0.713725507f, 0.756862819f, 1.000000000f };
		constexpr Float4 light_salmon { 1.000000000f, 0.627451003f, 0.478431404f, 1.000000000f };
		constexpr Float4 light_sea_green { 0.125490203f, 0.698039234f, 0.666666687f, 1.000000000f };
		constexpr Float4 light_sky_blue { 0.529411793f, 0.807843208f, 0.980392218f, 1.000000000f };
		constexpr Float4 light_slate_gray { 0.466666698f, 0.533333361f, 0.600000024f, 1.000000000f };
		constexpr Float4 light_steel_blue { 0.690196097f, 0.768627524f, 0.870588303f, 1.000000000f };
		constexpr Float4 light_yellow { 1.000000000f, 1.000000000f, 0.878431439f, 1.000000000f };
		constexpr Float4 lime { 0.000000000f, 1.000000000f, 0.000000000f, 1.000000000f };
		constexpr Float4 lime_green { 0.196078449f, 0.803921640f, 0.196078449f, 1.000000000f };
		constexpr Float4 linen { 0.980392218f, 0.941176534f, 0.901960850f, 1.000000000f };
		constexpr Float4 magenta { 1.000000000f, 0.000000000f, 1.000000000f, 1.000000000f };
		constexpr Float4 maroon { 0.501960814f, 0.000000000f, 0.000000000f, 1.000000000f };
		constexpr Float4 medium_aqua_marine { 0.400000036f, 0.803921640f, 0.666666687f, 1.000000000f };
		constexpr Float4 medium_blue { 0.000000000f, 0.000000000f, 0.803921640f, 1.000000000f };
		constexpr Float4 medium_orchid { 0.729411781f, 0.333333343f, 0.827451050f, 1.000000000f };
		constexpr Float4 medium_purple { 0.576470613f, 0.439215720f, 0.858823597f, 1.000000000f };
		constexpr Float4 medium_sea_green { 0.235294133f, 0.701960802f, 0.443137288f, 1.000000000f };
		constexpr Float4 medium_slate_blue { 0.482352972f, 0.407843173f, 0.933333397f, 1.000000000f };
		constexpr Float4 medium_spring_green { 0.000000000f, 0.980392218f, 0.603921592f, 1.000000000f };
		constexpr Float4 medium_turquoise { 0.282352954f, 0.819607913f, 0.800000072f, 1.000000000f };
		constexpr Float4 medium_violet_red { 0.780392230f, 0.082352944f, 0.521568656f, 1.000000000f };
		constexpr Float4 midnight_blue { 0.098039225f, 0.098039225f, 0.439215720f, 1.000000000f };
		constexpr Float4 mint_cream { 0.960784376f, 1.000000000f, 0.980392218f, 1.000000000f };
		constexpr Float4 misty_rose { 1.000000000f, 0.894117713f, 0.882353008f, 1.000000000f };
		constexpr Float4 moccasin { 1.000000000f, 0.894117713f, 0.709803939f, 1.000000000f };
		constexpr Float4 navajo_white { 1.000000000f, 0.870588303f, 0.678431392f, 1.000000000f };
		constexpr Float4 navy { 0.000000000f, 0.000000000f, 0.501960814f, 1.000000000f };
		constexpr Float4 old_lace { 0.992156923f, 0.960784376f, 0.901960850f, 1.000000000f };
		constexpr Float4 olive { 0.501960814f, 0.501960814f, 0.000000000f, 1.000000000f };
		constexpr Float4 olive_drab { 0.419607878f, 0.556862772f, 0.137254909f, 1.000000000f };
		constexpr Float4 orange { 1.000000000f, 0.647058845f, 0.000000000f, 1.000000000f };
		constexpr Float4 orange_red { 1.000000000f, 0.270588249f, 0.000000000f, 1.000000000f };
		constexpr Float4 orchid { 0.854902029f, 0.439215720f, 0.839215755f, 1.000000000f };
		constexpr Float4 pale_golden_rod { 0.933333397f, 0.909803987f, 0.666666687f, 1.000000000f };
		constexpr Float4 pale_green { 0.596078455f, 0.984313786f, 0.596078455f, 1.000000000f };
		constexpr Float4 pale_turquoise { 0.686274529f, 0.933333397f, 0.933333397f, 1.000000000f };
		constexpr Float4 pale_violet_red { 0.858823597f, 0.439215720f, 0.576470613f, 1.000000000f };
		constexpr Float4 papaya_whip { 1.000000000f, 0.937254965f, 0.835294187f, 1.000000000f };
		constexpr Float4 peach_puff { 1.000000000f, 0.854902029f, 0.725490212f, 1.000000000f };
		constexpr Float4 peru { 0.803921640f, 0.521568656f, 0.247058839f, 1.000000000f };
		constexpr Float4 pink { 1.000000000f, 0.752941251f, 0.796078503f, 1.000000000f };
		constexpr Float4 plum { 0.866666734f, 0.627451003f, 0.866666734f, 1.000000000f };
		constexpr Float4 powder_blue { 0.690196097f, 0.878431439f, 0.901960850f, 1.000000000f };
		constexpr Float4 purple { 0.501960814f, 0.000000000f, 0.501960814f, 1.000000000f };
		constexpr Float4 red { 1.000000000f, 0.000000000f, 0.000000000f, 1.000000000f };
		constexpr Float4 rosy_brown { 0.737254918f, 0.560784340f, 0.560784340f, 1.000000000f };
		constexpr Float4 royal_blue { 0.254901975f, 0.411764741f, 0.882353008f, 1.000000000f };
		constexpr Float4 saddle_brown { 0.545098066f, 0.270588249f, 0.074509807f, 1.000000000f };
		constexpr Float4 salmon { 0.980392218f, 0.501960814f, 0.447058856f, 1.000000000f };
		constexpr Float4 sandy_brown { 0.956862807f, 0.643137276f, 0.376470625f, 1.000000000f };
		constexpr Float4 sea_green { 0.180392161f, 0.545098066f, 0.341176480f, 1.000000000f };
		constexpr Float4 sea_shell { 1.000000000f, 0.960784376f, 0.933333397f, 1.000000000f };
		constexpr Float4 sienna { 0.627451003f, 0.321568638f, 0.176470593f, 1.000000000f };
		constexpr Float4 silver { 0.752941251f, 0.752941251f, 0.752941251f, 1.000000000f };
		constexpr Float4 sky_blue { 0.529411793f, 0.807843208f, 0.921568692f, 1.000000000f };
		constexpr Float4 slate_blue { 0.415686309f, 0.352941185f, 0.803921640f, 1.000000000f };
		constexpr Float4 slate_gray { 0.439215720f, 0.501960814f, 0.564705908f, 1.000000000f };
		constexpr Float4 snow { 1.000000000f, 0.980392218f, 0.980392218f, 1.000000000f };
		constexpr Float4 spring_green { 0.000000000f, 1.000000000f, 0.498039246f, 1.000000000f };
		constexpr Float4 steel_blue { 0.274509817f, 0.509803951f, 0.705882370f, 1.000000000f };
		constexpr Float4 tan { 0.823529482f, 0.705882370f, 0.549019635f, 1.000000000f };
		constexpr Float4 teal { 0.000000000f, 0.501960814f, 0.501960814f, 1.000000000f };
		constexpr Float4 thistle { 0.847058892f, 0.749019623f, 0.847058892f, 1.000000000f };
		constexpr Float4 tomato { 1.000000000f, 0.388235331f, 0.278431386f, 1.000000000f };
		constexpr Float4 turquoise { 0.250980407f, 0.878431439f, 0.815686345f, 1.000000000f };
		constexpr Float4 violet { 0.933333397f, 0.509803951f, 0.933333397f, 1.000000000f };
		constexpr Float4 wheat { 0.960784376f, 0.870588303f, 0.701960802f, 1.000000000f };
		constexpr Float4 white { 1.000000000f, 1.000000000f, 1.000000000f, 1.000000000f };
		constexpr Float4 white_smoke { 0.960784376f, 0.960784376f, 0.960784376f, 1.000000000f };
		constexpr Float4 yellow { 1.000000000f, 1.000000000f, 0.000000000f, 1.000000000f };
		constexpr Float4 yellow_green { 0.603921592f, 0.803921640f, 0.196078449f, 1.000000000f };
	}

	//! u32 color format, each 8-bit component represents a color channel.
	using color_u32 = u32;

	//! Adjust color saturation.
	inline Float4 color_adj_saturation(const Float4& c, f32 sat)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		Float4 result;
		vec_float_t co = load_float4(c.m);
		store_float4(result.m, ColorAdjustSaturation(co, sat));
		return result;
#else
		constexpr Float4 gvLuminance(0.2125f, 0.7154f, 0.0721f, 0.0f);
		Float4 result;
		f32 fLuminance = (c.r * gvLuminance.r) + (c.g * gvLuminance.g) + (c.b * gvLuminance.b);
		result.r = ((c.r - fLuminance) * sat) + fLuminance;
		result.g = ((c.g - fLuminance) * sat) + fLuminance;
		result.b = ((c.b - fLuminance) * sat) + fLuminance;
		result.a = c.a;
		return result;
#endif
	}

	//! Adjust color contrast.
	inline Float4 color_adj_contrast(const Float4& c, f32 contrast)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		Float4 result;
		vec_float_t co = load_float4(c.m);
		store_float4(result.m, ColorAdjustContrast(co, contrast));
		return result;
#else
		Float4 result;
		result.r = ((c.r - 0.5f) * contrast) + 0.5f;
		result.g = ((c.g - 0.5f) * contrast) + 0.5f;
		result.b = ((c.b - 0.5f) * contrast) + 0.5f;
		result.a = c.a;
		return result;
#endif
	}

	//! Invert a color value.
	inline Float4 color_negate(const Float4& c)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		Float4 r;
		vec_float_t vecc = load_float4(c.m);
		store_float4(r.m, ColorNegative(vecc));
#else
		Float4 result;
		result.r = 1.0f - c.r;
		result.g = 1.0f - c.g;
		result.b = 1.0f - c.b;
		result.a = c.a;
		return result;
#endif
	}

	//! Convert float color to RGBA8 format.
	inline color_u32 color_to_rgba8(const Float4& c)
	{
		return
			u32(c.r * 255.0f) << 24 |
			u32(c.g * 255.0f) << 16 |
			u32(c.b * 255.0f) << 8 |
			u32(c.a * 255.0f);
	}

	inline color_u32 color_to_argb8(const Float4& c)
	{
		return
			u32(c.a * 255.0f) << 24 |
			u32(c.r * 255.0f) << 16 |
			u32(c.g * 255.0f) << 8 |
			u32(c.b * 255.0f);
	}

	inline color_u32 color_to_abgr8(const Float4& c)
	{
		return
			u32(c.a * 255.0f) << 24 |
			u32(c.b * 255.0f) << 16 |
			u32(c.g * 255.0f) << 8 |
			u32(c.r * 255.0f);
	}

	//! Convert RGBA8 color to float color.
	inline Float4 rgba8_to_color(color_u32 c)
	{
		Float4 re;
		re.r = (f32)((c & 0xff000000) >> 24);
		re.g = (f32)((c & 0x00ff0000) >> 16);
		re.b = (f32)((c & 0x0000ff00) >> 8);
		re.a = (f32)((c & 0x000000ff));
		re /= 255.0f;
		return re;
	}

	inline Float4 argb8_to_color(color_u32 c)
	{
		Float4 re;
		re.a = (f32)((c & 0xff000000) >> 24);
		re.r = (f32)((c & 0x00ff0000) >> 16);
		re.g = (f32)((c & 0x0000ff00) >> 8);
		re.b = (f32)((c & 0x000000ff));
		re /= 255.0f;
		return re;
	}

	inline Float4 abgr8_to_color(color_u32 c)
	{
		Float4 re;
		re.a = (f32)((c & 0xff000000) >> 24);
		re.b = (f32)((c & 0x00ff0000) >> 16);
		re.g = (f32)((c & 0x0000ff00) >> 8);
		re.r = (f32)((c & 0x000000ff));
		re /= 255.0f;
		return re;
	}
}