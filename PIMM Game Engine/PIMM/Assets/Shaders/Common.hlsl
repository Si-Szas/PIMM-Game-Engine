struct VS_INPUT
{
    float3 position : POSITION0;
    float2 texcoord : TEXCOORD0;
};

struct VS_OUTPUT
{
    float3 position : POSITION0;
    float2 texcoord : TEXCOORD0;
};

/// QUAD PATCHES ///
#define QUAD_CONTROL_POINTS 4
struct HS_CONSTANT_QUAD_OUTPUT
{
    float edgeTessFactor[4] : SV_TessFactor;
    float insideTessFactor[2] : SV_InsideTessFactor;
};

/// TRI PATCHES ///
#define TRI_CONTROL_POINTS 3
struct HS_CONSTANT_TRI_OUTPUT
{
    float edgeTessFactor[3]  : SV_TessFactor;
    float insideTessFactor   : SV_InsideTessFactor;
};

struct HS_INPUT
{
    float3 position : POSITION0;
    float2 texcoord : TEXCOORD0;
};

struct HS_OUTPUT
{
    float3 position : POSITION0;
    float2 texcoord : TEXCOORD0;
};

struct DS_OUTPUT
{
    float4 position : SV_Position;
    float2 texcoord : TEXCOORD0;
};

cbuffer ObjectData : register(b0)
{
    row_major float4x4 world;
}

cbuffer CameraData : register(b1)
{
    row_major float4x4 view;
    row_major float4x4 projection;
}

cbuffer MaterialData : register(b2)
{
    float3 materialColor;
}

sampler DefaultSampler : register(s0);
