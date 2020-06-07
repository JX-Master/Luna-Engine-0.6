cbuffer CameraParams : register(b0)
{
    float4x4 world_to_view;
    float4x4 view_to_proj;
    float4x4 world_to_proj;
    float4x4 view_to_world;
    float4 env_light_color;
};

struct MeshParams
{
    float4x4 model_to_world;
    float4x4 world_to_model;
};

struct LightParams
{
    float3 strength;
    float attenuation_power;    // Only for point / spot light.

    float3 direction;   // Only for directional / spot light.
    uint type;          // 0 = directional, 1 = point, 2 = spot.

    float3 position;    // Only for point / spot light. In world space.
    float spot_attenuation_power;   // Only for spot light.
};

struct PSInput
{
    float4 position : SV_POSITION;
    float3 normal   : NORMAL;
    float3 tangent  : TANGENT;
    float2 texcoord : TEXCOORD;
    float4 color    : COLOR;
    float3 world_position : POSITION;
};

StructuredBuffer<MeshParams> g_mesh_params : register(t0);
StructuredBuffer<LightParams> g_light_params : register(t1);
Texture2D g_base_color : register(t2);
Texture2D g_roughness : register(t3);
Texture2D g_normal : register(t4);
Texture2D g_metallic : register(t5);
Texture2D g_emissive : register(t6);
Texture2D g_skybox : register(t7);
SamplerState g_sampler : register(s0);

static const float PI = 3.1415926f;

float4 sample_sky(float3 direction)
{
    float h = direction.y;
    float texcoord_v = 0.5f - asin(h) / PI; //(PI / 2.0 - asin(H)) / PI;
    float2 ortho = normalize(float2(direction.x, direction.z));
    float ang_h = acos(ortho.x);
    float texcoord_h = ang_h / 2.0f / PI;
    texcoord_h = ortho.y < 0.0f ? 1.0f - texcoord_h : texcoord_h;
    return g_skybox.Sample(g_sampler, float2(texcoord_h, texcoord_v));
}

float3 normal_tangent_to_world(float3 normal_map, float3 normal_world, float3 tangent_world)
{
    float3 n = normal_world;
    float3 t = normalize(tangent_world - dot(tangent_world, n) * n);
    float3 b = cross(n, t);
    float3x3 tbn = float3x3(t, b, n);
    return mul(normal_map, tbn);
}

float3 diffuse_term(float3 diffuse_color, float n_dot_v, float n_dot_l, float l_dot_h, float roughness)
{
    float fd90 = 0.5f + 2.0f * l_dot_h * l_dot_h * roughness;
    float light_scatter = (1.0f + (fd90 - 1.0f) * pow(1.0f - n_dot_l, 5));
    float view_scatter = (1.0f + (fd90 - 1.0f) * pow(1.0f - n_dot_v, 5));
    return diffuse_color * (light_scatter * view_scatter) / PI;
}

float3 fresnel_schlick(float3 specular_color, float l_dot_h)
{
    float t = pow(1.0f - l_dot_h, 5);
    return specular_color + (1.0f - specular_color) * t;
}

float3 fresnel_lerp(float3 specular_color_0, float3 specular_color_1, float l_dot_h)
{
    float t = pow(1.0f - l_dot_h, 5);
    return lerp(specular_color_0, specular_color_1, t);
}

float ggx_vis(float n_dot_l, float n_dot_v, float roughness)
{
    float a2 = roughness * roughness;
    float lambda_v = n_dot_l * (n_dot_v * (1.0f - a2) + a2);
    float lambda_l = n_dot_v * (n_dot_l * (1.0f - a2) + a2);
    return 0.5f / (lambda_v + lambda_l + 0.00001f); // Prevent deviding by 0.
}

float ggx_normal_distrb(float n_dot_h, float roughness)
{
    float a4 = roughness * roughness;
    a4 = a4 * a4;
    float d = (n_dot_h * a4 - n_dot_h) * n_dot_h + 1.0f;
    return a4 / (d * d + 0.00001f) / PI;
}

float3 specular_term(float3 specular_color, float roughness, float n_dot_l, float n_dot_v, float n_dot_h, float l_dot_h)
{
    float v = ggx_vis(n_dot_l, n_dot_v, roughness);
    float d = ggx_normal_distrb(n_dot_h, roughness);
    float3 f = fresnel_schlick(specular_color, l_dot_h);
    return f * v * d;
}

float2 uv_latlong(float3 dir)
{
    dir = normalize(dir);
    float H = dir.y;

    float texcoordV = 0.5f - asin(H) / PI; //(PI / 2.0 - asin(H)) / PI;

    float2 ortho = normalize(float2(dir.x, dir.z));

    float angH = acos(ortho.x);

    float texcoordH = 0.0f;
    if (ortho.y < 0)
    {
        texcoordH = 1.0f - (angH / 2.0f / PI);
    }
    else
    {
        texcoordH = angH / 2.0f / PI;
    }
    return float2(texcoordH, texcoordV);
}

float4 main(PSInput i) : SV_Target
{
    float2 texcoord = float2(i.texcoord.x, 1.0f - i.texcoord.y);

    // Sample every texture.
    float4 base_color = g_base_color.Sample(g_sampler, texcoord);
    float roughness = g_roughness.Sample(g_sampler, texcoord).x;
    float3 normal = normalize(g_normal.Sample(g_sampler, texcoord).xyz - 0.5f);
    float metallic = g_metallic.Sample(g_sampler, texcoord).x;
    float4 emissive = g_emissive.Sample(g_sampler, texcoord);

    // Calculates per-model informations.
    float3 base_normal = normalize(i.normal);
    float3 base_tangent = normalize(i.tangent);
    float4 camera_pos_world = mul(view_to_world, float4(0.0f, 0.0f, 0.0f, 1.0f));
    float3 view_dir = normalize(camera_pos_world.xyz - i.world_position);   // From frag to camera.

    // Apply normal map to model normal.
    normal = normal_tangent_to_world(normal, base_normal, base_tangent);

    uint num_lights;
    uint light_stride;
    g_light_params.GetDimensions(num_lights, light_stride);

    float3 diffuse_color = base_color.xyz * (1.0f - metallic);
    float3 specular_color = lerp(float3(0.03f, 0.03f, 0.03f), base_color.xyz, metallic);

    float nv = saturate(dot(normal, view_dir));

    float3 final_color = 0.0f;

    // Calculates lights contribution.
    for(uint index = 0; index < num_lights; ++index)
    {
        float3 light_color = g_light_params[index].strength;
        float3 light_dir; // frag to light.
        if(g_light_params[index].type == 0)  // directional.
        {
            light_dir = -normalize(g_light_params[index].direction);
        }
        else
        {
            light_dir = -normalize(i.world_position - g_light_params[index].position);
        }
        // Apply distance attenuation for point and spot light.
        if(g_light_params[index].type != 0)
        {
            float dist = distance(i.world_position, g_light_params[index].position);
            float attenuation_power = max(g_light_params[index].attenuation_power, 0.000001f);
            float attentation = (dist / attenuation_power) * (dist / attenuation_power) + 1.0f;
            light_color /= attentation;
        }
        // Apply spot attenuation for spot light.
        if(g_light_params[index].type == 2)
        {
            float spot_attentation = pow(max(dot(-light_dir, g_light_params[index].direction), 0.0f), g_light_params[index].spot_attenuation_power);
            light_color *= spot_attentation;
        }

        float3 half_dir = normalize(light_dir + view_dir);
        float nl = saturate(dot(normal, light_dir));
        float nh = saturate(dot(normal, half_dir));
        float lh = saturate(dot(light_dir, half_dir));

        // Calculate diffuse term.
        // float3 diffuse_color, float n_dot_v, float n_dot_l, float l_dot_h, float roughness
        float3 diffuse = diffuse_term(diffuse_color, nv, nl, lh, roughness);

        // Calculate specular term.
        //! float3 specular_color, float roughness, float n_dot_l, float n_dot_v, float n_dot_h, float l_dot_h
        float3 specular = specular_term(specular_color, roughness, nl, nv, nh, lh);

        final_color += PI * (diffuse + specular) * light_color * nl;
    }

    // IBL Specular.
    float perceptual_roughness = roughness * (1.7f - 0.7f * roughness);
    float env_mip = perceptual_roughness * 6;
    float3 refl_dir = reflect(-view_dir, normal);
    float2 env_uv = uv_latlong(refl_dir);
    float4 env_color = g_skybox.SampleLevel(g_sampler, env_uv, env_mip);
    float one_minus_reflectivity = 1.0f - max(max(specular_color.r, specular_color.g), specular_color.b);
    float grazing_term = saturate((1.0f - roughness) + (1.0f - one_minus_reflectivity));
    float surface_reduction = 1.0f / (roughness * roughness + 1.0f);
    float3 ibl_specular = surface_reduction * env_color.rgb * fresnel_lerp(specular_color, grazing_term, nv);

    // Composition.
    final_color += emissive.xyz + ibl_specular + env_light_color.xyz * diffuse_color;
    return float4(final_color, 1.0f);
}