#include "Particle.hlsli"
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

StructuredBuffer<ParticleMatrix> gParticleMatrix : register(t0);
ConstantBuffer<ViewProjectionMatrix> gViewProjectionMatrix : register(b1);

ConstantBuffer<InstanceLocation> gInstanceLocation : register(b2);
ConstantBuffer<ModelTransfomMatrix> gModelTransMat : register(b4);

struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input, uint instanceId : SV_InstanceID)
{
    VertexShaderOutput output;
    
    uint targetLocation = instanceId + gInstanceLocation.location;
    
    float4x4 matWorld = mul(gModelTransMat.modelMat_, gParticleMatrix[targetLocation].World);
    
    matrix matVP = mul(gViewProjectionMatrix.view, gViewProjectionMatrix.projection);
    matrix matWVP = mul(matWorld, matVP);
    
    output.position = mul(input.position, matWVP);
    output.texCoord = input.texCoord;
    output.normal = normalize(mul(input.normal, (float3x3) matWorld));
    output.color = gParticleMatrix[targetLocation].color;
    return output;
}