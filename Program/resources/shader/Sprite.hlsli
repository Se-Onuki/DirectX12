struct ConstData
{
    float4 color;
    matrix matWorldProjection;
};

ConstantBuffer<ConstData> gConstData : register(b0);

struct VertexShaderOutput
{
    float4 svPosition : SV_POSITION;
    float2 texCoord : TEXCOORD;
};