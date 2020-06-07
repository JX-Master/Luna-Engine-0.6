Texture2D g_src_tex : register(t0);
RWTexture2D<float> g_dest_tex : register(u0);

[numthreads(8, 8, 1)]
void main(int3 dispatch_thread_id : SV_DispatchThreadID)
{
	float key = 0.0f;
	key += g_src_tex[dispatch_thread_id.xy * 2].x;
	key += g_src_tex[dispatch_thread_id.xy * 2 + int2(1, 0)].x;
	key += g_src_tex[dispatch_thread_id.xy * 2 + int2(0, 1)].x;
	key += g_src_tex[dispatch_thread_id.xy * 2 + int2(1, 1)].x;
	key /= 4.0f;
	g_dest_tex[dispatch_thread_id.xy] = key;
}