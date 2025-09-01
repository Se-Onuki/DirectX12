#include "Object3d.hlsli"
#include "Math.hlsli"

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

struct Well
{
    float4x4 skeletonSpaceMatrix_;
    float4x4 skeletonSpaceInverseTransponeMatrix_;
};

StructuredBuffer<Well> gMatrixPalette : register(t1);

struct ModelTransfomMatrix
{
    float4x4 modelMat_;
};

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);
ConstantBuffer<ViewProjectionMatrix> gViewProjectionMatrix : register(b3);

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
    
    result.position = (
        0.f, 0.f, 0.f, 0.f
    );
    for (int j = 0; j < 4; j++)
    {
        result.position += mul(input.position, gMatrixPalette[input.index[j]].skeletonSpaceMatrix_) * input.weight[j];
    }
    result.position.w = 1.f; // 確実に1を代入する
    
    // 法線の変換
    for (int i = 0; i < 3; i++)
    {
        result.normal += mul(input.normal, (float3x3) gMatrixPalette[input.index[i]].skeletonSpaceInverseTransponeMatrix_) * input.weight[i];
    }
    // 正規化しておく
    result.normal = normalize(result.normal);
    
    return result;
}

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    Skinned skinned = Skinning(input); // スキニング計算を行い、頂点情報を取得する。結果はローカル頂点座標
    
    // float4x4 mulWorld = mul(gModelTransMat.modelMat_, gTransformationMatrix.World);
    float4x4 mulWorld = gTransformationMatrix.World;
    matrix matVP = mul(gViewProjectionMatrix.view, gViewProjectionMatrix.projection);
    matrix matWVP = mul(mulWorld, matVP);
    output.position = mul(skinned.position, matWVP);
    output.texCoord = input.texCoord;
    output.normal = normalize(mul(skinned.normal, (float3x3) gTransformationMatrix.WorldInverseTranspose));
    output.worldPos = mul(skinned.position, mulWorld).xyz;
    return output;
}