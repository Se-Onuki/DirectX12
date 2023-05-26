#include "Object3d.hlsli"
struct TransfirmationMatrix
{
    float32_t4x4 WVP;
};
ConstantBuffer<TransfirmationMatrix> gTransformationMatrix : register(b0);

struct VertexShaderInput
{
    float32_t4 position : POSITION0;
    float32_t2 texCoord : TEXCOORD0;
    
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    output.position = mul(input.position, gTransformationMatrix.WVP);
    output.texCoord = input.texCoord;
    return output;
}