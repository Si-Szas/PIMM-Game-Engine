// FROM DOMAIN SHADER //
struct DS_OUTPUT
{
    float4 position : SV_Position;
    float4 color : COLOR0;
};
// END OF FROM DOMAIN SHADER

float4 PS_Main(DS_OUTPUT input) : SV_Target
{
    return input.color;
}