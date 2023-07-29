#include "Object3d.hlsli"
struct Material
{
    float4 color;
    float4 emissive;

    float4x4 uvTransform;
};


struct DirectionalLight
{
    float4 color; // 色(RGBA)
    float3 direction; // ライトの向き
    float intensity; // 輝度
    
    uint pattern;
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
    
    
    float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
    if (gDirectionalLight.pattern == 0)
    {
        output.color = textureColor;
        output.color.w = gMaterial.color.w * textureColor.w; // α値
        
    }
    else if (gDirectionalLight.pattern == 1)
    {
        output.color = gMaterial.color * gDirectionalLight.color * NdotL * gDirectionalLight.intensity; // 
        output.color = saturate(output.color + gMaterial.emissive) * textureColor; // 自己発光
        output.color.w = gMaterial.color.w * textureColor.w; // α値
    }
    else if (gDirectionalLight.pattern == 2)
    {
        float cos = pow(NdotL * 0.5f + 0.5f, 2.f);
        output.color = gMaterial.color * gDirectionalLight.color * cos * gDirectionalLight.intensity; // 
        output.color = saturate(output.color + gMaterial.emissive) * textureColor; // 自己発光
        output.color.w = gMaterial.color.w * textureColor.w; // α値
    }
   
    return output;
}