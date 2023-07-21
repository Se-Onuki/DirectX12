#pragma once
#include <vector>
#include "../Render/Render.hpp"

#include <string>
#include <fstream>
#include <sstream>

#include <list>
#include <unordered_map>

#include "../Math/Vector4.h"
#include "../Math/Vector3.h"
#include "../Math/Vector2.h"
#include "../Math/Matrix4x4.h"
#include <wrl.h>
#include <d3d12.h>

struct Transform;
class ViewProjection;

struct Material {

private:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public:

	struct MaterialData {
		Vector4 color;			// 色(RGBA)
		int32_t enableLighting;	// ライティングの有無
	private:
		float patting[3];
	public:
		Matrix4x4 uvTransform;
	};

	Vector3 ambient_;			// アンビエント影響度
	Vector3 diffuse_;			// ディフューズ影響度
	Vector3 specular_;			// スペキュラー影響度
	std::string name_;			// マテリアル名
	std::string textureName_;
	uint32_t texHandle_ = 1u;
	ComPtr<ID3D12Resource> materialBuff_;

	static Material LoadFile(const std::string &directoryPath, const std::string &fileName);
};

struct Mesh
{
private:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public:

	struct VertexData {
		Vector4 position;	// 頂点位置
		Vector2 texCoord;	// UV座標系
		Vector3 normal;		// 法線

		// 比較。すべてが一致した場合のみ真を返す
		bool operator==(const VertexData &vertex) const {
			return position == vertex.position && texCoord == vertex.texCoord && normal == vertex.normal;
		}
	};

	D3D12_VERTEX_BUFFER_VIEW vbView_ = {};
	D3D12_INDEX_BUFFER_VIEW ibView_ = {};

	ComPtr<ID3D12Resource> vertexBuff_;
	ComPtr<ID3D12Resource> indexBuff_;

	std::vector<VertexData> vertices_;
	std::vector<uint32_t> indexs_;
	Material material_;

	void CreateBuffer();

	void AddVertex(const VertexData &vertex);

	void SetMaterial(Material *const material);

	void Draw(ID3D12GraphicsCommandList *const commandList, const Transform &transform) const;

	static void CreateSphere(VertexData *const vertex,
		ID3D12Resource *const indexResource, const uint32_t &subdivision);

};


class Model
{
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public:
	Model();
	~Model();

	std::string name_;
	std::vector<Mesh *> meshList_;
	std::unordered_map<std::string, std::unique_ptr<Material>> materialMap_;

	//static void Init(ID3D12GraphicsCommandList *const commandList);

	void LoadObjFile(const std::string &directoryPath, const std::string &fileName);

	void Draw(const Transform &transform, const ViewProjection &viewProjection) const;

	static void StartDraw(ID3D12GraphicsCommandList *const commandList);
	static void EndDraw();
private:
	static ID3D12GraphicsCommandList *commandList_;

};
