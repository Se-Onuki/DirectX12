
struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 worldPos : POSITION0;
};

struct ModelTransfomMatrix
{
    float4x4 modelMat_;
};