#include "Sprite.hlsli"

struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texCoord : TEXCOORD0;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    output.svPosition = mul(input.position, gConstData.matWorldProjection);
    output.texCoord = input.texCoord;
    return output;
}