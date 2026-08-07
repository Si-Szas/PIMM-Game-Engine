#include "PIMM/Assets/Shaders/Common.hlsl"

Texture2D Diffuse : register(t0);

VS_OUTPUT VS_Main(VS_INPUT input)
{
    VS_OUTPUT output;
    
    output.position = input.position;
    //output.color = input.color;
    output.texcoord = input.texcoord;
    
    return output;
}

HS_CONSTANT_TRI_OUTPUT CalcTriHSPatchConstants(
    InputPatch<HS_INPUT, TRI_CONTROL_POINTS> ip,
    uint PatchID : SV_PrimitiveID)
{
    HS_CONSTANT_TRI_OUTPUT output;

    output.edgeTessFactor[0] = 
    output.edgeTessFactor[1] = 
    output.edgeTessFactor[2] = 
    output.insideTessFactor  = 5.0f;
    
    return output;
}

[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(TRI_CONTROL_POINTS)]
[patchconstantfunc("CalcTriHSPatchConstants")]
HS_OUTPUT HS_Main(
    InputPatch<HS_INPUT, TRI_CONTROL_POINTS> ip,
    uint i : SV_OutputControlPointID,
    uint patchID : SV_PrimitiveID)
{
    HS_OUTPUT output;

    output.position = ip[i].position;
    output.texcoord = ip[i].texcoord;

    return output;
}

[domain("tri")]
DS_OUTPUT DS_Main(
    HS_CONSTANT_TRI_OUTPUT input,
    float3 uvw : SV_DomainLocation,
    const OutputPatch<HS_OUTPUT, TRI_CONTROL_POINTS> patch)
{
    DS_OUTPUT output;
    
    float3 finalPosition = patch[0].position * uvw.x + 
                           patch[1].position * uvw.y + 
                           patch[2].position * uvw.z;
    
    float4 worldPosition = mul(float4(finalPosition, 1.0f), world);
    float4 viewPosition = mul(worldPosition, view);
    float4 projectionPosition = mul(viewPosition, projection);
    output.position = projectionPosition;
    
    output.texcoord = patch[0].texcoord * uvw.x + 
                      patch[1].texcoord * uvw.y + 
                      patch[2].texcoord * uvw.z;
    
    return output;
}

float4 PS_Main(DS_OUTPUT input) : SV_Target
{
    float4 diffuse = Diffuse.Sample(DefaultSampler, input.texcoord);

    //If texture does not exist, then render using the objects's material color
    if (diffuse.a == 0.0f)
    {
        return float4(materialColor.rgb, 1.0f);
    }

    return float4(materialColor.rgb * diffuse.rgb, 1.0f);
}