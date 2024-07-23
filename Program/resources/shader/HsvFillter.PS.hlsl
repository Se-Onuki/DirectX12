#include "FullScreen.hlsli"
#include "Math.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct HsvParam
{
    float hue_;
    float saturation_;
    float value_;
    float pad_;
};
ConstantBuffer<HsvParam> gHsvParam : register(b0);

struct PixelShaderOutput
{
    float4 color_ : SV_Target0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    // 色を取得する
    float4 textureColor = gTexture.Sample(gSampler, input.texCoord_);
    // 色のHSV変換    
    float3 hsv = 0;
    hsv = RGBtoHSV(textureColor.rgb);
    
    // 調整    
    hsv.x = WrapValue01(hsv.x + gHsvParam.hue_);
    hsv.y = saturate(hsv.y + gHsvParam.saturation_);
    hsv.z = saturate(hsv.z + gHsvParam.value_);
    
    // RGBに戻す
    float3 rgb = HSVtoRGB(hsv);
    
    PixelShaderOutput result;
    
    result.color_ = float4(rgb, textureColor.a);
    
    return result;
    
}