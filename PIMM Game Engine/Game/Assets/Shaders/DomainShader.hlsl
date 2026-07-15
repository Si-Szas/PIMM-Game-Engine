#define NUMBER_CONTROL_POINTS 4

// FROM HULL SHADER //
struct HS_CONSTANT_DATA_OUTPUT
{
    float edgeTessFactor[4] : SV_TessFactor;
    float insideTessFactor[2] : SV_InsideTessFactor;
};

struct HS_OUTPUT
{
    float3 position : POSITION0;
    float4 color : COLOR0;
};
// END OF FROM HULL SHADER //

struct DS_OUTPUT
{
    float4 position : SV_Position;
    float4 color : COLOR0;
};

cbuffer MaterialData : register(b2)
{
    float3 materialColor;
}

[domain("quad")]
DS_OUTPUT DS_Main(
	HS_CONSTANT_DATA_OUTPUT input,
	float2 domain : SV_DomainLocation,
	const OutputPatch<HS_OUTPUT, NUMBER_CONTROL_POINTS> patch)
{
    DS_OUTPUT output;
    
    float3 top = lerp(patch[0].position, patch[1].position, domain.x);
    float3 bottom = lerp(patch[2].position, patch[3].position, domain.x);
    float3 finalPosition = lerp(top, bottom, domain.y);
    
    float4 worldPosition = mul(float4(finalPosition, 1.0f), world);
    float4 viewPosition = mul(worldPosition, view);
    float4 projectionPosition = mul(viewPosition, projection);
    output.position = projectionPosition;
    
    float4 topColor = lerp(patch[0].color, patch[1].color, domain.x);
    float4 bottomColor = lerp(patch[2].color, patch[3].color, domain.x);
    output.color = lerp(topColor, bottomColor, domain.y);
    
    return output;
}