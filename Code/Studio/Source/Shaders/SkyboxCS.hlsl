Texture2D g_skybox : register(t0);
SamplerState g_sampler : register(s0);
RWTexture2D<float4> g_lighting_tex : register(u0);

cbuffer SkyboxParams : register(b0)
{
	float4x4 g_view_to_world;
	float g_fov;
	uint g_width;
	uint g_height;
}


// asin [-1,1] -> [-PI/2, PI/2]
// acos [-1,1] -> [0, PI]
// atan -> [-PI/2, PI/2]
[numthreads(8, 8, 1)]
void main(int3 dispatch_thread_id : SV_DispatchThreadID)
{
	float focus_len = g_width / (2.0f * tan(g_fov / 2.0f));

	float3 world_dir = normalize(float3((float)dispatch_thread_id.x - (float)(g_width / 2), -((float)dispatch_thread_id.y - (float)(g_height / 2)), focus_len));
	world_dir = mul(g_view_to_world, float4(world_dir, 0.0f)).xyz;

	// Sample the texture.
	const float PI = 3.1415926;
	float h = world_dir.y;
	float texcoord_v = 0.5f - asin(h) / PI; //(PI / 2.0 - asin(H)) / PI;
	float2 ortho = normalize(float2(world_dir.x, world_dir.z));
	float ang_h = acos(ortho.x);
	float texcoord_h = ang_h / 2.0f / PI;
	texcoord_h = ortho.y < 0.0f ? 1.0f - texcoord_h : texcoord_h;
	float4 src_color = g_skybox.SampleLevel(g_sampler, float2(texcoord_h, texcoord_v), 0.0f);
	g_lighting_tex[dispatch_thread_id.xy] = float4(src_color.rgb, 1.0f);
	//g_lighting_tex[dispatch_thread_id.xy] = float4(max(world_dir.rgb, float3(0.0f, 0.0f, 0.0f)), 1.0f);
}