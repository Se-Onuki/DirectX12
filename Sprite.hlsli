struct ConstData
{
    float4 color;
    matrix matWorldProjection;
};

ConstantBuffer<ConstData> gConstData : register(b0);

struct VertexShaderOutput
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD;
};