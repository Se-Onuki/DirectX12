#include "Object3d.hlsli"
struct Material
{
    float32_t4 color;
    int32_t enableLighting;
};
struct DirectionLight
{
    float32_t4 color; // 色(RGBA)
    float32_t3 direction; // ライトの向き
    float intensity; // 輝度
};
ConstantBuffer<Material> gMaterial : register(b0);
//ConstantBuffer<DirectionLight> gDirectionLight : register(b1);

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float32_t4 textureColor = gTexture.Sample(gSampler, input.texCoord);
    
    //if (gMaterial.enableLighting != 0)
    //{
    //    float cos = saturate(dot(normalize(input.normal), -gDirectionLight.direction));
    //    output.color = gMaterial.color * textureColor * gDirectionLight.color * cos * gDirectionLight.intensity;
    //}
    //else
    //{
        output.color = gMaterial.color * textureColor;
    //}
    return output;
}