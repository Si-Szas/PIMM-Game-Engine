#define NUMBER_CONTROL_POINTS 4

struct HS_INPUT
{
    float3 position : POSITION0;
    float4 color : COLOR0;
};

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