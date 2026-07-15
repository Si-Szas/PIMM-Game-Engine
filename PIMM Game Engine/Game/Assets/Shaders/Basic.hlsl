#include "PIMM/Assets/Shaders/Common.hlsl"

VS_OUTPUT VS_Main(VS_INPUT input)
{
    VS_OUTPUT output;
    
    output.position = input.position;
    output.color = float4(materialColor, 1.0f);
    
    return output;
}

HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(
	InputPatch<HS_INPUT, NUMBER_CONTROL_POINTS> ip,
	uint PatchID : SV_PrimitiveID)
{
    HS_CONSTANT_DATA_OUTPUT output;

    output.edgeTessFactor[0] =
    output.edgeTessFactor[1] =
    output.edgeTessFactor[2] =
    output.edgeTessFactor[3] =
    output.insideTessFactor[0] =
    output.insideTessFactor[1] = 5.0f;
    
    return output;
}

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(NUMBER_CONTROL_POINTS)]
[patchconstantfunc("CalcHSPatchConstants")]
HS_OUTPUT HS_Main(
	InputPatch<HS_INPUT, NUMBER_CONTROL_POINTS> ip,
	uint i : SV_OutputControlPointID,
	uint patchID : SV_PrimitiveID)
{
    HS_OUTPUT output;

    output.position = ip[i].position;
    output.color = ip[i].color;

    return output;
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

float4 PS_Main(DS_OUTPUT input) : SV_Target
{
    return input.color;
}