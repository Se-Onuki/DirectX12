#include "FullScreen.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct VignetteParam
{
    float first;
    float second;
};
ConstantBuffer<VignetteParam> gVignetteParam : register(b0);

struct PixelShaderOutput
{
    float32_t4 color_ : SV_Target0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput result;
    result.color_ = gTexture.Sample(gSampler, input.texCoord_);
    float2 correct = input.texCoord_ * (1.f - input.texCoord_.yx);
    float vignette = correct.x * correct.y * gVignetteParam.first;
    vignette = saturate(pow(vignette, gVignetteParam.second));

    result.color_.rgb *= vignette;
    
    return result;
}