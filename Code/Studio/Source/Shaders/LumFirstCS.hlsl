cbuffer ToneMappingCB : register(b0)
{
	float4 offset4x4[16];
}

Texture2D g_src_tex : register(t0);
RWTexture2D<float> g_dest_tex : register(u0);
SamplerState g_sampler : register(s0);

float lum(float3 p)
{
	return 0.2125f * p.r + 0.7154f * p.g + 0.0721f * p.b;
}

[numthreads(8, 8, 1)]
void main(int3 dispatch_thread_id : SV_DispatchThreadID)
{
	float k = 0.0f;
	float4 c = 0.0f;
	float2 uv = float2(dispatch_thread_id.xy) / 1024.0f;
	for (int i = 0; i < 16; ++i)
	{
		c = g_src_tex.SampleLevel(g_sampler, uv + offset4x4[i].xy, 0.0f);
		k += log(lum(c.xyz) + 0.0001);
	}
	k /= 16.0f;
	g_dest_tex[dispatch_thread_id.xy] = k;
}