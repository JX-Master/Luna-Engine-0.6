// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Color.hpp
* @author JXMaster
* @date 2019/1/5
*/
#pragma once
#include "Vector.hpp"

namespace luna
{
	//! Color type is just a aliasing of Float4.
	using Color = Float4;
	using ColorU = Float4U;

	// Regularly used colors.
	// Reference : http://www.w3school.com.cn/tiy/color.asp
	namespace color
	{
		constexpr Color alice_blue{ 0.941176534f, 0.972549081f, 1.000000000f, 1.000000000f };
		constexpr Color antique_white { 0.980392218f, 0.921568692f, 0.843137324f, 1.000000000f };
		constexpr Color aqua { 0.000000000f, 1.000000000f, 1.000000000f, 1.000000000f };
		constexpr Color aquamarine { 0.498039246f, 1.000000000f, 0.831372619f, 1.000000000f };
		constexpr Color azure { 0.498039246f, 1.000000000f, 0.831372619f, 1.000000000f };
		constexpr Color beige { 0.960784376f, 0.960784376f, 0.862745166f, 1.000000000f };
		constexpr Color bisque { 1.000000000f, 0.894117713f, 0.768627524f, 1.000000000f };
		constexpr Color black { 0.000000000f, 0.000000000f, 0.000000000f, 1.000000000f };
		constexpr Color blanched_almond { 1.000000000f, 0.921568692f, 0.803921640f, 1.000000000f };
		constexpr Color blue { 0.000000000f, 0.000000000f, 1.000000000f, 1.000000000f };
		constexpr Color blue_violet { 0.541176498f, 0.168627456f, 0.886274576f, 1.000000000f };
		constexpr Color brown { 0.647058845f, 0.164705887f, 0.164705887f, 1.000000000f };
		constexpr Color burly_wood { 0.870588303f, 0.721568644f, 0.529411793f, 1.000000000f };
		constexpr Color cadet_blue { 0.372549027f, 0.619607866f, 0.627451003f, 1.000000000f };
		constexpr Color chartreuse { 0.498039246f, 1.000000000f, 0.000000000f, 1.000000000f };
		constexpr Color chocolate { 0.823529482f, 0.411764741f, 0.117647067f, 1.000000000f };
		constexpr Color coral { 1.000000000f, 0.498039246f, 0.313725501f, 1.000000000f };
		constexpr Color cornflower_blue { 0.392156899f, 0.584313750f, 0.929411829f, 1.000000000f };
		constexpr Color cornsilk { 1.000000000f, 0.972549081f, 0.862745166f, 1.000000000f };
		constexpr Color crimson { 0.862745166f, 0.078431375f, 0.235294133f, 1.000000000f };
		constexpr Color cyan { 0.000000000f, 1.000000000f, 1.000000000f, 1.000000000f };
		constexpr Color dark_blue { 0.000000000f, 0.000000000f, 0.545098066f, 1.000000000f };
		constexpr Color dark_cyan { 0.000000000f, 0.545098066f, 0.545098066f, 1.000000000f };
		constexpr Color dark_golden_rod { 0.721568644f, 0.525490224f, 0.043137256f, 1.000000000f };
		constexpr Color dark_gray { 0.662745118f, 0.662745118f, 0.662745118f, 1.000000000f };
		constexpr Color dark_green { 0.000000000f, 0.392156899f, 0.000000000f, 1.000000000f };
		constexpr Color dark_khaki { 0.741176486f, 0.717647076f, 0.419607878f, 1.000000000f };
		constexpr Color dark_magenta { 0.545098066f, 0.000000000f, 0.545098066f, 1.000000000f };
		constexpr Color dark_olive_green { 0.333333343f, 0.419607878f, 0.184313729f, 1.000000000f };
		constexpr Color dark_orange { 1.000000000f, 0.549019635f, 0.000000000f, 1.000000000f };
		constexpr Color dark_orchid { 0.600000024f, 0.196078449f, 0.800000072f, 1.000000000f };
		constexpr Color dark_red { 0.545098066f, 0.000000000f, 0.000000000f, 1.000000000f };
		constexpr Color dark_salmon { 0.913725555f, 0.588235319f, 0.478431404f, 1.000000000f };
		constexpr Color dark_sea_green { 0.560784340f, 0.737254918f, 0.545098066f, 1.000000000f };
		constexpr Color dark_slate_blue { 0.282352954f, 0.239215702f, 0.545098066f, 1.000000000f };
		constexpr Color dark_slate_gray { 0.184313729f, 0.309803933f, 0.309803933f, 1.000000000f };
		constexpr Color dark_turquoise { 0.000000000f, 0.807843208f, 0.819607913f, 1.000000000f };
		constexpr Color dark_violet { 0.580392182f, 0.000000000f, 0.827451050f, 1.000000000f };
		constexpr Color deep_pink { 1.000000000f, 0.078431375f, 0.576470613f, 1.000000000f };
		constexpr Color deep_sky_blue { 0.000000000f, 0.749019623f, 1.000000000f, 1.000000000f };
		constexpr Color dim_gray { 0.411764741f, 0.411764741f, 0.411764741f, 1.000000000f };
		constexpr Color dodger_blue { 0.117647067f, 0.564705908f, 1.000000000f, 1.000000000f };
		constexpr Color fire_brick { 0.698039234f, 0.133333340f, 0.133333340f, 1.000000000f };
		constexpr Color floral_white { 1.000000000f, 0.980392218f, 0.941176534f, 1.000000000f };
		constexpr Color forest_green { 0.133333340f, 0.545098066f, 0.133333340f, 1.000000000f };
		constexpr Color fuchsia { 1.000000000f, 0.000000000f, 1.000000000f, 1.000000000f };
		constexpr Color gainsboro { 0.862745166f, 0.862745166f, 0.862745166f, 1.000000000f };
		constexpr Color ghost_white { 0.972549081f, 0.972549081f, 1.000000000f, 1.000000000f };
		constexpr Color gold { 1.000000000f, 0.843137324f, 0.000000000f, 1.000000000f };
		constexpr Color golden_rod { 0.854902029f, 0.647058845f, 0.125490203f, 1.000000000f };
		constexpr Color gray { 0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f };
		constexpr Color green { 0.000000000f, 0.501960814f, 0.000000000f, 1.000000000f };
		constexpr Color green_yellow { 0.678431392f, 1.000000000f, 0.184313729f, 1.000000000f };
		constexpr Color honey_dew { 0.941176534f, 1.000000000f, 0.941176534f, 1.000000000f };
		constexpr Color hot_pink { 1.000000000f, 0.411764741f, 0.705882370f, 1.000000000f };
		constexpr Color indian_red { 0.803921640f, 0.360784322f, 0.360784322f, 1.000000000f };
		constexpr Color indigo { 0.294117659f, 0.000000000f, 0.509803951f, 1.000000000f };
		constexpr Color ivory { 1.000000000f, 1.000000000f, 0.941176534f, 1.000000000f };
		constexpr Color khaki { 0.941176534f, 0.901960850f, 0.549019635f, 1.000000000f };
		constexpr Color lavender { 0.901960850f, 0.901960850f, 0.980392218f, 1.000000000f };
		constexpr Color lavender_blush { 1.000000000f, 0.941176534f, 0.960784376f, 1.000000000f };
		constexpr Color lawn_green { 0.486274540f, 0.988235354f, 0.000000000f, 1.000000000f };
		constexpr Color lemon_chiffon { 1.000000000f, 0.980392218f, 0.803921640f, 1.000000000f };
		constexpr Color light_blue { 0.678431392f, 0.847058892f, 0.901960850f, 1.000000000f };
		constexpr Color light_coral { 0.941176534f, 0.501960814f, 0.501960814f, 1.000000000f };
		constexpr Color light_cyan { 0.878431439f, 1.000000000f, 1.000000000f, 1.000000000f };
		constexpr Color light_golden_rod_yellow { 0.980392218f, 0.980392218f, 0.823529482f, 1.000000000f };
		constexpr Color light_gray { 0.827451050f, 0.827451050f, 0.827451050f, 1.000000000f };
		constexpr Color light_green { 0.564705908f, 0.933333397f, 0.564705908f, 1.000000000f };
		constexpr Color light_pink { 1.000000000f, 0.713725507f, 0.756862819f, 1.000000000f };
		constexpr Color light_salmon { 1.000000000f, 0.627451003f, 0.478431404f, 1.000000000f };
		constexpr Color light_sea_green { 0.125490203f, 0.698039234f, 0.666666687f, 1.000000000f };
		constexpr Color light_sky_blue { 0.529411793f, 0.807843208f, 0.980392218f, 1.000000000f };
		constexpr Color light_slate_gray { 0.466666698f, 0.533333361f, 0.600000024f, 1.000000000f };
		constexpr Color light_steel_blue { 0.690196097f, 0.768627524f, 0.870588303f, 1.000000000f };
		constexpr Color light_yellow { 1.000000000f, 1.000000000f, 0.878431439f, 1.000000000f };
		constexpr Color lime { 0.000000000f, 1.000000000f, 0.000000000f, 1.000000000f };
		constexpr Color lime_green { 0.196078449f, 0.803921640f, 0.196078449f, 1.000000000f };
		constexpr Color linen { 0.980392218f, 0.941176534f, 0.901960850f, 1.000000000f };
		constexpr Color magenta { 1.000000000f, 0.000000000f, 1.000000000f, 1.000000000f };
		constexpr Color maroon { 0.501960814f, 0.000000000f, 0.000000000f, 1.000000000f };
		constexpr Color medium_aqua_marine { 0.400000036f, 0.803921640f, 0.666666687f, 1.000000000f };
		constexpr Color medium_blue { 0.000000000f, 0.000000000f, 0.803921640f, 1.000000000f };
		constexpr Color medium_orchid { 0.729411781f, 0.333333343f, 0.827451050f, 1.000000000f };
		constexpr Color medium_purple { 0.576470613f, 0.439215720f, 0.858823597f, 1.000000000f };
		constexpr Color medium_sea_green { 0.235294133f, 0.701960802f, 0.443137288f, 1.000000000f };
		constexpr Color medium_slate_blue { 0.482352972f, 0.407843173f, 0.933333397f, 1.000000000f };
		constexpr Color medium_spring_green { 0.000000000f, 0.980392218f, 0.603921592f, 1.000000000f };
		constexpr Color medium_turquoise { 0.282352954f, 0.819607913f, 0.800000072f, 1.000000000f };
		constexpr Color medium_violet_red { 0.780392230f, 0.082352944f, 0.521568656f, 1.000000000f };
		constexpr Color midnight_blue { 0.098039225f, 0.098039225f, 0.439215720f, 1.000000000f };
		constexpr Color mint_cream { 0.960784376f, 1.000000000f, 0.980392218f, 1.000000000f };
		constexpr Color misty_rose { 1.000000000f, 0.894117713f, 0.882353008f, 1.000000000f };
		constexpr Color moccasin { 1.000000000f, 0.894117713f, 0.709803939f, 1.000000000f };
		constexpr Color navajo_white { 1.000000000f, 0.870588303f, 0.678431392f, 1.000000000f };
		constexpr Color navy { 0.000000000f, 0.000000000f, 0.501960814f, 1.000000000f };
		constexpr Color old_lace { 0.992156923f, 0.960784376f, 0.901960850f, 1.000000000f };
		constexpr Color olive { 0.501960814f, 0.501960814f, 0.000000000f, 1.000000000f };
		constexpr Color olive_drab { 0.419607878f, 0.556862772f, 0.137254909f, 1.000000000f };
		constexpr Color orange { 1.000000000f, 0.647058845f, 0.000000000f, 1.000000000f };
		constexpr Color orange_red { 1.000000000f, 0.270588249f, 0.000000000f, 1.000000000f };
		constexpr Color orchid { 0.854902029f, 0.439215720f, 0.839215755f, 1.000000000f };
		constexpr Color pale_golden_rod { 0.933333397f, 0.909803987f, 0.666666687f, 1.000000000f };
		constexpr Color pale_green { 0.596078455f, 0.984313786f, 0.596078455f, 1.000000000f };
		constexpr Color pale_turquoise { 0.686274529f, 0.933333397f, 0.933333397f, 1.000000000f };
		constexpr Color pale_violet_red { 0.858823597f, 0.439215720f, 0.576470613f, 1.000000000f };
		constexpr Color papaya_whip { 1.000000000f, 0.937254965f, 0.835294187f, 1.000000000f };
		constexpr Color peach_puff { 1.000000000f, 0.854902029f, 0.725490212f, 1.000000000f };
		constexpr Color peru { 0.803921640f, 0.521568656f, 0.247058839f, 1.000000000f };
		constexpr Color pink { 1.000000000f, 0.752941251f, 0.796078503f, 1.000000000f };
		constexpr Color plum { 0.866666734f, 0.627451003f, 0.866666734f, 1.000000000f };
		constexpr Color powder_blue { 0.690196097f, 0.878431439f, 0.901960850f, 1.000000000f };
		constexpr Color purple { 0.501960814f, 0.000000000f, 0.501960814f, 1.000000000f };
		constexpr Color red { 1.000000000f, 0.000000000f, 0.000000000f, 1.000000000f };
		constexpr Color rosy_brown { 0.737254918f, 0.560784340f, 0.560784340f, 1.000000000f };
		constexpr Color royal_blue { 0.254901975f, 0.411764741f, 0.882353008f, 1.000000000f };
		constexpr Color saddle_brown { 0.545098066f, 0.270588249f, 0.074509807f, 1.000000000f };
		constexpr Color salmon { 0.980392218f, 0.501960814f, 0.447058856f, 1.000000000f };
		constexpr Color sandy_brown { 0.956862807f, 0.643137276f, 0.376470625f, 1.000000000f };
		constexpr Color sea_green { 0.180392161f, 0.545098066f, 0.341176480f, 1.000000000f };
		constexpr Color sea_shell { 1.000000000f, 0.960784376f, 0.933333397f, 1.000000000f };
		constexpr Color sienna { 0.627451003f, 0.321568638f, 0.176470593f, 1.000000000f };
		constexpr Color silver { 0.752941251f, 0.752941251f, 0.752941251f, 1.000000000f };
		constexpr Color sky_blue { 0.529411793f, 0.807843208f, 0.921568692f, 1.000000000f };
		constexpr Color slate_blue { 0.415686309f, 0.352941185f, 0.803921640f, 1.000000000f };
		constexpr Color slate_gray { 0.439215720f, 0.501960814f, 0.564705908f, 1.000000000f };
		constexpr Color snow { 1.000000000f, 0.980392218f, 0.980392218f, 1.000000000f };
		constexpr Color spring_green { 0.000000000f, 1.000000000f, 0.498039246f, 1.000000000f };
		constexpr Color steel_blue { 0.274509817f, 0.509803951f, 0.705882370f, 1.000000000f };
		constexpr Color tan { 0.823529482f, 0.705882370f, 0.549019635f, 1.000000000f };
		constexpr Color teal { 0.000000000f, 0.501960814f, 0.501960814f, 1.000000000f };
		constexpr Color thistle { 0.847058892f, 0.749019623f, 0.847058892f, 1.000000000f };
		constexpr Color tomato { 1.000000000f, 0.388235331f, 0.278431386f, 1.000000000f };
		constexpr Color turquoise { 0.250980407f, 0.878431439f, 0.815686345f, 1.000000000f };
		constexpr Color violet { 0.933333397f, 0.509803951f, 0.933333397f, 1.000000000f };
		constexpr Color wheat { 0.960784376f, 0.870588303f, 0.701960802f, 1.000000000f };
		constexpr Color white { 1.000000000f, 1.000000000f, 1.000000000f, 1.000000000f };
		constexpr Color white_smoke { 0.960784376f, 0.960784376f, 0.960784376f, 1.000000000f };
		constexpr Color yellow { 1.000000000f, 1.000000000f, 0.000000000f, 1.000000000f };
		constexpr Color yellow_green { 0.603921592f, 0.803921640f, 0.196078449f, 1.000000000f };
	}



	//! Uint32 color format, each 8-bit component represents a color channel.
	using color_u32 = uint32;

	//! Adjust color saturation.
	inline Float4 color_adj_saturation(const Float4& c, float32 sat)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace simd;
		Float4 result;
		vec_float_t co = load_float4(c.m);
		store_float4(result.m, ColorAdjustSaturation(co, sat));
		return result;
#else
		constexpr Float4 gvLuminance(0.2125f, 0.7154f, 0.0721f, 0.0f);
		Float4 result;
		float32 fLuminance = (c.r * gvLuminance.r) + (c.g * gvLuminance.g) + (c.b * gvLuminance.b);
		result.r = ((c.r - fLuminance) * sat) + fLuminance;
		result.g = ((c.g - fLuminance) * sat) + fLuminance;
		result.b = ((c.b - fLuminance) * sat) + fLuminance;
		result.a = c.a;
		return result;
#endif
	}

	//! Adjust color contrast.
	inline Float4 color_adj_contrast(const Float4& c, float32 contrast)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace simd;
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
		using namespace simd;
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
	inline color_u32 color_to_rgba8(const Color& c)
	{
		return
			uint32(c.r * 255.0f) << 24 |
			uint32(c.g * 255.0f) << 16 |
			uint32(c.b * 255.0f) << 8 |
			uint32(c.a * 255.0f);
	}

	inline color_u32 color_to_argb8(const Color& c)
	{
		return
			uint32(c.a * 255.0f) << 24 |
			uint32(c.r * 255.0f) << 16 |
			uint32(c.g * 255.0f) << 8 |
			uint32(c.b * 255.0f);
	}

	inline color_u32 color_to_abgr8(const Color& c)
	{
		return
			uint32(c.a * 255.0f) << 24 |
			uint32(c.b * 255.0f) << 16 |
			uint32(c.g * 255.0f) << 8 |
			uint32(c.r * 255.0f);
	}

	//! Convert RGBA8 color to float color.
	inline Color rgba8_to_color(color_u32 c)
	{
		Color re;
		re.r = (float32)((c & 0xff000000) >> 24);
		re.g = (float32)((c & 0x00ff0000) >> 16);
		re.b = (float32)((c & 0x0000ff00) >> 8);
		re.a = (float32)((c & 0x000000ff));
		re /= 255.0f;
		return re;
	}

	inline Color argb8_to_color(color_u32 c)
	{
		Color re;
		re.a = (float32)((c & 0xff000000) >> 24);
		re.r = (float32)((c & 0x00ff0000) >> 16);
		re.g = (float32)((c & 0x0000ff00) >> 8);
		re.b = (float32)((c & 0x000000ff));
		re /= 255.0f;
		return re;
	}

	inline Color abgr8_to_color(color_u32 c)
	{
		Color re;
		re.a = (float32)((c & 0xff000000) >> 24);
		re.b = (float32)((c & 0x00ff0000) >> 16);
		re.g = (float32)((c & 0x0000ff00) >> 8);
		re.r = (float32)((c & 0x000000ff));
		re /= 255.0f;
		return re;
	}
}