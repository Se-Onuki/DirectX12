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

struct InstanceLocation {
    uint location;
};

StructuredBuffer<ParticleMatrix> gParticleMatrix : register(t0);
ConstantBuffer<ViewProjectionMatrix> gViewProjectionMatrix : register(b1);

ConstantBuffer<InstanceLocation> gInstanceLocation : register(b2);

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
    
    matrix matVP = mul(gViewProjectionMatrix.view, gViewProjectionMatrix.projection);
    matrix matWVP = mul(gParticleMatrix[targetLocation].World, matVP);
    
    output.position = mul(input.position, matWVP);
    output.texCoord = input.texCoord;
    output.normal = normalize(mul(input.normal, (float3x3) gParticleMatrix[targetLocation].World));
    output.color = gParticleMatrix[targetLocation].color;
    return output;
}