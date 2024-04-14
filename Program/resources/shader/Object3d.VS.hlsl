#include "Object3d.hlsli"
#include "Math.hlsli"

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

struct ModelTransfomMatrix
{
    float4x4 modelMat_;
};

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);
ConstantBuffer<ViewProjectionMatrix> gViewProjectionMatrix : register(b3);
ConstantBuffer<ModelTransfomMatrix> gModelTransMat : register(b4);

struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    float4x4 mulWorld = mul(gModelTransMat.modelMat_, gTransformationMatrix.World);
    //float4x4 mulWorld = gTransformationMatrix.World;
    matrix matVP = mul(gViewProjectionMatrix.view, gViewProjectionMatrix.projection);
    matrix matWVP = mul(mulWorld, matVP);
    output.position = mul(input.position, matWVP);
    output.texCoord = input.texCoord;
    output.normal = normalize(mul(input.normal, InverseTranspone((float3x3) mulWorld)));
    output.worldPos = mul(input.position, mulWorld).xyz;
    return output;
}