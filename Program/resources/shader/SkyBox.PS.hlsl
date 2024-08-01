#include "SkyBox.hlsli"

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

TextureCube<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput result;
    float4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    result.color = textureColor;
    return result;
}