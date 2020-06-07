Texture2D g_scene_tex : register(t0);
Texture2D g_lum_tex : register(t1);
RWTexture2D<float4> g_dest_tex : register(u0);

cbuffer g_cb : register(b0)
{
	float g_exposure;
}

[numthreads(8, 8, 1)]
void main(int3 dispatch_thread_id : SV_DispatchThreadID)
{
	float4 adjusted_color = g_scene_tex[dispatch_thread_id.xy];
	float lum_value = exp(g_lum_tex[int2(0, 0)].x);
	adjusted_color = adjusted_color / (lum_value + 0.0001f);
	float4 final = adjusted_color / (adjusted_color + g_exposure + 0.0001f);
	g_dest_tex[dispatch_thread_id.xy] = float4(saturate(final.xyz), 1.0);
}