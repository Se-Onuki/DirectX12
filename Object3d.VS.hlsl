struct TransfirmationMatrix
{
    float32_t4x4 WVP;
};
ConstantBuffer<TransfirmationMatrix> gTransformationMatrix : register(b0);

struct VertexShaderOutput
{
    float32_t4 position : SV_POSITION;
};
    
struct VertexShaderInput
{
    float32_t4 position : POSITION0;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    output.position = mul(input.position, gTransformationMatrix.WVP);
    return output;
}