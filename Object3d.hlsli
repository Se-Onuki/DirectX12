
struct VertexShaderOutput
{
	float32_t4 position : SV_POSITION;
	float32_t2 texCoord : TEXCOORD0;
	float32_t3 normal : NORMAL0;
};