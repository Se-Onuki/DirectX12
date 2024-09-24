#include "Particle.hlsli"
#include "Math.hlsli"

struct ParticleMatrix
{
    matrix World;
    float4 color;
};

struct ViewProjectionMatrix
{
    matrix view;
    matrix projection;
    float3 cameraPos;
};

struct InstanceLocation
{
    uint location;
};

struct ModelTransfomMatrix
{
    float4x4 modelMat_;
};

struct Well
{
    float4x4 skeletonSpaceMatrix_;
    float4x4 skeletonSpaceInverseTransponeMatrix_;
};

StructuredBuffer<ParticleMatrix> gParticleMatrix : register(t0);
StructuredBuffer<Well> gMatrixPalette : register(t1);

ConstantBuffer<ViewProjectionMatrix> gViewProjectionMatrix : register(b3);

ConstantBuffer<InstanceLocation> gInstanceLocation : register(b2);
ConstantBuffer<ModelTransfomMatrix> gModelTransMat : register(b4);

struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL0;
    
    float4 weight : WEIGHT0;
    uint4 index : INDEX0;
};

struct Skinned
{
    float4 position;
    float3 normal;
};

Skinned Skinning(VertexShaderInput input)
{
    
    Skinned result;
    // スキニングの処理
    
    result.position = 0.f;
    
    for (int i = 0; i < 4; i++)
    {
        result.position += mul(input.position, gMatrixPalette[input.index[i]].skeletonSpaceMatrix_) * input.weight[i];
    }
    result.position.w = 1.f; // 確実に1を代入する
    
    result.normal = 0.f;

    // 法線の変換
    for (i = 0; i < 3; i++)
    {
        result.normal += mul(input.normal, InverseTranspone((float3x3) (gMatrixPalette[input.index[i]].skeletonSpaceMatrix_))) * input.weight[i];
    }
    // 正規化しておく
    result.normal = normalize(result.normal);
    
    return result;
}
VertexShaderOutput main(VertexShaderInput input, uint instanceId : SV_InstanceID)
{
    VertexShaderOutput output;
    Skinned skinned = Skinning(input);
    
    uint targetLocation = instanceId + gInstanceLocation.location;
    
    float4x4 matWorld = mul(gModelTransMat.modelMat_, gParticleMatrix[targetLocation].World);
    
    matrix matVP = mul(gViewProjectionMatrix.view, gViewProjectionMatrix.projection);
    matrix matWVP = mul(matWorld, matVP);
    
    output.position = mul(skinned.position, matWVP);
    output.texCoord = input.texCoord;
    output.normal = normalize(mul(skinned.normal, InverseTranspone((float3x3) matWorld)));
    output.color = gParticleMatrix[targetLocation].color;
    output.worldPos = mul(input.position, matWorld).xyz;
    return output;
}