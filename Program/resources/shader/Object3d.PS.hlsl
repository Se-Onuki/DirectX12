#include "Object3d.hlsli"
#include "Math.hlsli"

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

struct ViewProjectionMatrix
{
    matrix view;
    matrix projection;
    float3 cameraPos;
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
ConstantBuffer<ViewProjectionMatrix> gViewProjectionMatrix : register(b3);

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
    
    // 色が不透明の場合破棄
    if (textureColor.a < 0.9f)
    {
        discard;
    }
    
    float3 toEye = normalize(gViewProjectionMatrix.cameraPos - input.worldPos);
    float3 halfVector = normalize(-gDirectionalLight.direction + toEye);
    float NDotH = dot(normalize(input.normal), halfVector);
    float specularPow = saturate(pow(NDotH, gMaterial.shininess));
    
    //float3 reflectLight = reflect(normalize(gDirectionalLight.direction), normalize(input.normal));
    
    //float RditE = dot(reflectLight, normalize(toEye));
    //float specularPow = pow(saturate(RditE), gMaterial.shininess * 100.f); // 反射強度
    
    float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
    if (gDirectionalLight.pattern == 0)
    {
        output.color.rgb = textureColor.rgb;
        output.color.a = gMaterial.color.a * textureColor.a; // α値
        
    }
    else if (gDirectionalLight.pattern == 1)
    {
        output.color.rgb = saturate(gMaterial.color.rgb * gDirectionalLight.color.rgb * NdotL * gDirectionalLight.intensity); // 拡散反射
        output.color.rgb = saturate(output.color.rgb + gMaterial.emissive.rgb) * textureColor.rgb; // 自己発光
        output.color.a = gMaterial.color.a * textureColor.a; // α値
    }
    else if (gDirectionalLight.pattern == 2)
    {
        float cos = pow(NdotL * 0.5f + 0.5f, 2.f);
        float3 diffuse = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity; // 拡散反射
        
        float3 emissive = saturate(gMaterial.emissive.rgb); // 自己発光
        float3 specular = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float3(1.0f, 1.0f, 1.0f) * gMaterial.shininessStrength;
        
        output.color.rgb = saturate(saturate(diffuse) + saturate(emissive) + saturate(specular));
        
        output.color.a = gMaterial.color.a * textureColor.a; // α値
    }
    
    // 色が不透明の場合破棄
    if (output.color.a < 0.1f)
    {
        discard;
    }
    
    return output;
}