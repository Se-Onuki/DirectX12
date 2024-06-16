#include "Particle.hlsli"
struct Material
{
    float4 color;
    float4 emissive;
    float4 ambient;

    float4x4 uvTransform;
    float shininess;
    float shininessStrength;
};


struct DirectionalLight
{
    float4 color; // 色(RGBA)
    float3 direction; // ライトの向き
    float intensity; // 輝度
    
    int pattern;
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float4 transformedUV = mul(float4(input.texCoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    
    output.color = input.color * textureColor; 
        
    // 色が不透明の場合破棄
    if (output.color.a == 0.f)
    {
        discard;
    }
    
    return output;
}