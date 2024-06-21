
struct Well
{
    float4x4 skeletonSpaceMatrix_;
    float4x4 skeletonSpaceInverseTransponeMatrix_;
};

struct Vertex
{
    float4 position_;
    float3 normal_;
    float2 texCoord_;
};

struct VertexInfluence
{
    float4 weight_;
    uint4 index_;
};

struct SkinningInformation
{
    uint numVertices_;
};


// SkinnningObject3d.VS.hlslのPaletteと同じ
StructuredBuffer<Well> gMatrixPalette : register(t0);
// 頂点情報 [0]
StructuredBuffer<Vertex> gInputVertices : register(t1);
// 頂点情報 [1]
StructuredBuffer<VertexInfluence> gInfluence : register(t2);
// 計算後の頂点データ
RWStructuredBuffer<Vertex> gOutputVertices : register(u0); // 書き込み対応
// Skinningに関する情報群
ConstantBuffer<SkinningInformation> gSkinningInformation : register(b0);

[numthreads(1024, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint vertexIndex = DTid.x;
    if (vertexIndex < gSkinningInformation.numVertices_)
    {
        // Calc Skinning

        // 必要なデータをStructuredBufferから取ってくる
        Vertex inputVtx = gInputVertices[vertexIndex];
        VertexInfluence influence = gInfluence[vertexIndex];
        
        // Skinning後の頂点を計算する
        Vertex result;
        result.texCoord_ = inputVtx.texCoord_;
        
        result.position_ = 0.f;
        for (int i = 0; i < 4; i++)
        {
            result.position_ += mul(inputVtx.position_, gMatrixPalette[influence.index_[i]].skeletonSpaceMatrix_) * influence.weight_[i];
        }
        result.position_.w = 1.f; // 確実に1を代入する
    
        // 法線の変換
        for (i = 0; i < 3; i++)
        {
            result.normal_ += mul(inputVtx.normal_, (float3x3) gMatrixPalette[influence.index_[i]].skeletonSpaceInverseTransponeMatrix_) * influence.weight_[i];
        }
        // 正規化しておく
        result.normal_ = normalize(result.normal_);
        
        
        // 結果を書き込む
        gOutputVertices[vertexIndex] = result;
        

    }
    
}