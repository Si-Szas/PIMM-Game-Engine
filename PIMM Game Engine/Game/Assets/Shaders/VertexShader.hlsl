struct VS_INPUT
{
    float3 position : POSITION0;
    float4 color : COLOR0;
};

struct VS_OUTPUT
{
    float3 position : POSITION0;
    float4 color : COLOR0;
};

VS_OUTPUT VS_Main(VS_INPUT input)
{
    VS_OUTPUT output;
    
    output.position = input.position;
    output.color = input.color;
    
    return output;
}
