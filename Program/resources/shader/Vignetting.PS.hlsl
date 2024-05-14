#include "FullScreen.hlsli"

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
    float2 correct = input.texCoord_ * (1.f - input.texCoord_.yx);
    float vignette = correct.x * correct.y * 16.f;
    vignette = saturate(pow(vignette, 0.8f));

    result.color_.rgb *= vignette;

    //float32_t value = dot(result.color_.rgb, float32_t3(0.2125f,0.7154f,0.0721f));
    //result.color_.rgb = float32_t3(value, value, value);
    
    return result;
}