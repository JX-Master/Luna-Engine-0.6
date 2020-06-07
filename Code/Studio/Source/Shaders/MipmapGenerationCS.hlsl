Texture2D<float4> g_src_tex : register(t0);
RWTexture2D<float4> g_dst_tex : register(u0);
SamplerState g_sampler : register(s0);

cbuffer CB : register(b0)
{
	float2 texel_size;	// 1.0 / destination dimension

}

[numthreads(8, 8, 1)]
void main(uint3 dispatch_thread_id : SV_DispatchThreadID)
{
	//DTid is the thread ID * the values from numthreads above and in this case correspond to the pixels location in number of pixels.
	//As a result texcoords (in 0-1 range) will point at the center between the 4 pixels used for the mipmap.
	float2 texcoords = texel_size * (dispatch_thread_id.xy + 0.5f);

	//The samplers linear interpolation will mix the four pixel values to the new pixels color
	float4 color = g_src_tex.SampleLevel(g_sampler, texcoords, 0.0f);

	//Write the final color into the destination texture.
	g_dst_tex[dispatch_thread_id.xy] = color;
}