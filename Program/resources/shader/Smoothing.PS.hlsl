#include "FullScreen.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct WindowSize
{
    int x;
    int y;
};
ConstantBuffer<WindowSize> gWindowSize : register(b0);

static const float2 kIndex3x3[3][3] =
{
    { { -1.f, -1.f }, { 0.f, -1.f }, { 1.f, -1.f } },
    { { -1.f, +0.f }, { 0.f, +0.f }, { 1.f, +0.f } },
    { { -1.f, +1.f }, { 0.f, +1.f }, { 1.f, +1.f } },

};

static const float kKernel3x3[3][3] =
{
    { 1.f / 9.f, 1.f / 9.f, 1.f / 9.f },
    { 1.f / 9.f, 1.f / 9.f, 1.f / 9.f },
    { 1.f / 9.f, 1.f / 9.f, 1.f / 9.f },
};

struct PixelShaderOutput
{
    float4 color_ : SV_Target0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput result;
    
    const float2 uvStepSize = float2(rcp((float) gWindowSize.x),
    rcp((float) gWindowSize.y));
    
    result.color_.rgb = float3(0.f, 0.f, 0.f);
    
    for (int xi = 0; xi < 3; xi++)
    {
        for (int yi = 0; yi < 3; yi++)
        {
            float2 texcoord = input.texCoord_ + kIndex3x3[xi][yi] * uvStepSize;
            // 色を1/9にして足す
            float3 fetchColor = gTexture.Sample(gSampler, texcoord).rgb;
            result.color_.rgb += fetchColor * kKernel3x3[xi][yi];
        }

    }
   
    return result;
}