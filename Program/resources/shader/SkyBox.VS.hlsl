#include "SkyBox.hlsli"

struct TransformationMatrix
{
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
ConstantBuffer<ViewProjectionMatrix> gViewProjectionMatrix : register(b1);

struct VertexShaderInput
{
    float4 position : POSITION0;
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