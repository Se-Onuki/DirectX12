#include "CopyImage.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float32_t4 color_ : SV_Target0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput result;
    result.color_ = gTexture.Sample(gSampler, input.texCoord_);
    float32_t value = dot(result.color_.rgb, float32_t3(0.2125f,0.7154f,0.0721f));
    result.color_.rgb = float32_t3(value, value, value);
    
    return result;
}