#include "Object3d.hlsli"
struct TransformationMatrix
{
    //float32_t4x4 WVP;
    float32_t4x4 World;
};
struct ViewProjectionMatrix
{
    //float32_t4x4 WVP;
    float32_t4x4 viewProjection;
};

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);
ConstantBuffer<ViewProjectionMatrix> gViewProjectionMatrix : register(b1);

struct VertexShaderInput
{
    float32_t4 position : POSITION0;
    float32_t2 texCoord : TEXCOORD0;
    float32_t3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    output.position = mul(input.position, mul(gTransformationMatrix.World, gViewProjectionMatrix.viewProjection));
    output.texCoord = input.texCoord;
    output.normal = normalize(mul(input.normal, (float32_t3x3) gTransformationMatrix.World));
    return output;
}