#include "FullScreen.hlsli"

static const float M_PI = 3.14159265358f;
static const int KERNEL_SIZE = 128;

float gauss(const float x, const float y, const float sigma)
{
    const float powSigma = sigma * sigma;
    const float exponent = -(x * x + y * y) * rcp(2.f * powSigma);
    const float denominator = 2.f * M_PI * powSigma;
    return exp(exponent) * rcp(denominator);
}
float gauss(const float x, const float sigma)
{
    return rcp (sqrt(2.0f * M_PI) * sigma) * exp(-(x * x) * rcp (2.0f * sigma * sigma));
}

//struct pixel_info
//{
//    sampler2D tex;
//    float2 uv;
//    float4 texelSize;
//};

//float4 GaussianBlurLinearSampling(pixel_info pinfo, float sigma, float2 dir)
//{
//    float4 o = 0;
//    float sum = 0;
//    float2 uvOffset;
//    float weight;

//    for (int kernelStep = -KERNEL_SIZE / 2; kernelStep <= KERNEL_SIZE / 2; kernelStep += 2)
//    {
//        uvOffset = pinfo.uv;
//        uvOffset.x += ((kernelStep + 0.5f) * pinfo.texelSize.x) * dir.x;
//        uvOffset.y += ((kernelStep + 0.5f) * pinfo.texelSize.y) * dir.y;
//        weight = gauss(kernelStep, sigma) + gauss(kernelStep + 1, sigma);
//        o += tex2D(pinfo.tex, uvOffset) * weight;
//        sum += weight;
//    }
//    o *= (1.0f / sum);
//    return o;
//}

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct GaussianParam
{
    float sigma_;
    int kernelSize_;
};
ConstantBuffer<GaussianParam> gGaussianParam : register(b0);

static const float2 kIndex3x3[3][3] =
{
    { { -1.f, -1.f }, { 0.f, -1.f }, { 1.f, -1.f } },
    { { -1.f, +0.f }, { 0.f, +0.f }, { 1.f, +0.f } },
    { { -1.f, +1.f }, { 0.f, +1.f }, { 1.f, +1.f } },

};

struct PixelShaderOutput
{
    float4 color_ : SV_Target0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput result;
    
    uint width, height;
    
    gTexture.GetDimensions(width, height);
    
    float3 o = 0.f;
    float sum = 0.f;
    
    const float rcpSize = rcp((float) width);
    
    const float sigma = gGaussianParam.sigma_;
    
    const int karnelHarf = gGaussianParam.kernelSize_ / 2;

    for (int kernelStep = -karnelHarf; kernelStep <= karnelHarf; kernelStep++)
    {
        float2 uvOffset = input.texCoord_;
        uvOffset.x += (kernelStep * rcpSize);
        
        float weight = gauss(kernelStep, sigma);
        o += gTexture.Sample(gSampler, uvOffset).rgb * weight;
        sum += weight;
    }
    o *= rcp(sum);
    
    result.color_ = float4(o, 1.f);

    return result;
}