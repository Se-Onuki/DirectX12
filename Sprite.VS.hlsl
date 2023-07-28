#include "Object3d.hlsli"

struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

float4 main(VertexShaderInput pos)
{
    return pos;
}