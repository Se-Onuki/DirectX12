#include "SkyBox.hlsli"


struct TransformationMatrix
{
    //matrix WVP;
    matrix World;
    matrix WorldInverseTranspose;
};
struct ViewProjectionMatrix
{
    matrix view;
    matrix projection;
    float3 cameraPos;
};

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);
ConstantBuffer<ViewProjectionMatrix> gViewProjectionMatrix : register(b3);

struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput result;
    const matrix matVP = mul(gViewProjectionMatrix.view, gViewProjectionMatrix.projection);
    const matrix matWVP = mul(gTransformationMatrix.World, matVP);
    result.position = mul(input.position, matWVP).xyww;
    result.texcoord = input.position.xyz;
    return result;
    
}