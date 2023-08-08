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

#include <array>

#include "../Math/Transform.h"

struct Transform;
class ViewProjection;

struct Material {

private:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public:
	Material() = default;
	Material(const std::string &materialName) {
		name_ = materialName;
	}

	struct MaterialData {
		Vector4 color;			// 色(RGBA)
		Vector4 emissive;		// 自己発光色(RGBA)
		Matrix4x4 uvTransform;
	};


	std::string name_;			// マテリアル名
	std::string textureName_;
	uint32_t texHandle_ = 1u;
	ComPtr<ID3D12Resource> constBuffer_;
	MaterialData *mapData_ = nullptr;

	void CreateBuffer();

	void ImGuiWidget();

	void Create();

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
	Material *material_;
	std::unordered_map<size_t, uint32_t> indexMap_;	// 頂点追加用一時データ

	void CreateBuffer();

	void AddVertex(const VertexData &vertex);

	void SetMaterial(Material *const material);

	void Draw(ID3D12GraphicsCommandList *const commandList) const;


};

namespace std {
	template<>
	struct hash<Mesh::VertexData> {
		size_t operator()(const Mesh::VertexData &v) const {
			std::string s =
				std::to_string(v.position.x) + "/" + std::to_string(v.position.y) + "/" + std::to_string(v.position.z) + "/" + std::to_string(v.position.w)	// 頂点
				+ "/" + std::to_string(v.texCoord.x) + "/" + std::to_string(v.texCoord.y)	// uv座標
				+ "/" + std::to_string(v.normal.x) + "/" + std::to_string(v.normal.y) + "/" + std::to_string(v.normal.z);	// 法線
			return std::hash<std::string>()(s);
		}
	};
}

class Model
{
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// モデル用パイプライン
	static std::array<ComPtr<ID3D12PipelineState>, 2u> graphicsPipelineState_;
	static ComPtr<ID3D12RootSignature> rootSignature_;

	static void CreatePipeLine();

public:
	enum class RootParameter {
		kWorldTransform, // ワールド変換行列
		kViewProjection, // ビュープロジェクション変換行列
		kMaterial,       // マテリアル
		kTexture,        // テクスチャ
		kLight,          // ライト
	};

	static void StaticInit();

	Model() = default;
	~Model() = default;

	std::string name_;
	std::vector<std::unique_ptr<Mesh>> meshList_;
	std::unordered_map<std::string, std::unique_ptr<Material>> materialMap_;


	void Draw(const Transform &transform, const ViewProjection &viewProjection) const;

	static void StartDraw(ID3D12GraphicsCommandList *const commandList);
	static void EndDraw();
	static const char *const defaultDirectory;

	void ImGuiWidget();

	[[nodiscard]] static Model *const CreateSphere();

	[[nodiscard]] static Model *const LoadObjFile(const std::string &directoryPath, const std::string &fileName);
private:
	void LoadMtlFile(const std::string &directoryPath, const std::string &fileName);

	static ID3D12GraphicsCommandList *commandList_;

};


class MinecraftModel {

	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	struct Face {
		D3D12_VERTEX_BUFFER_VIEW vbView_ = {};
		D3D12_INDEX_BUFFER_VIEW ibView_ = {};

		ComPtr<ID3D12Resource> vertexBuff_;
		ComPtr<ID3D12Resource> indexBuff_;

		Mesh::VertexData *vertices_;
		uint32_t *indexs_;

		void CreateBuffer();
		void Init();
		void SetVertex(const std::array<Vector3, 4u> &vertex, const Vector3 &normal);
		void Draw(ID3D12GraphicsCommandList *const commandList);
	};

	struct Cube {
		enum class FaceDirection : uint32_t {
			UP,
			Down,
			FRONT,
			BACK,
			RIGHT,
			LEFT,
			kCount
		};
		std::array<Face, (uint32_t)FaceDirection::kCount> faces_;
		Transform transformLocal_;
		void UpdateMatrix();

		void Init();
		void Draw(ID3D12GraphicsCommandList *const commandList);

		void SetVertex(const Vector3 &origin, const Vector3 &size);

		void ResetTransform();

	};

	struct Bone {
		std::string name_;
		std::vector<Cube> cubes_;
		std::unordered_map<std::string, Bone> children_;

		Bone *parent_;

		Transform transform_;

		void UpdateTransform();
		void SetParent(Bone *const parent);
		void Init();

		void AddCube();

		void Draw(ID3D12GraphicsCommandList *const commandList);
	};

public:
	Vector2 texelSize_ = {};
	uint32_t textureHandle_ = 1u;

	std::unordered_map<std::string, Bone> bones_ = {};
	std::unordered_map<std::string, Bone> children_{};

	MinecraftModel() = default;
	~MinecraftModel() = default;

	Transform transformOrigin_;

	void Init();
	void Draw(ID3D12GraphicsCommandList *const commandList);

	void LoadJson(const std::string &file_path);

};
