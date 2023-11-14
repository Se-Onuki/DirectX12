#pragma once
#include <vector>
#include "../Render/Camera.h"

#include <string>
#include <fstream>
#include <sstream>

#include <list>
#include <unordered_map>

#include "../../../Utils/Math/Vector4.h"
#include "../../../Utils/Math/Vector3.h"
#include "../../../Utils/Math/Vector2.h"
#include "../../../Utils/Math/Matrix4x4.h"

#include <wrl.h>
#include <d3d12.h>

#include <array>

#include "../../../Utils/Math/Transform.h"
#include "../Base/VertexBuffer.h"
#include "../Base/RootSignature.h"
#include "../Base/PipelineState.h"

#include "../Base/StructBuffer.h"
#include "../Descriptor/DescriptorManager.h"

class ViewProjection;

struct Material;
struct Mesh;

class Model {
public:
	enum class PipelineType : uint32_t {
		kModel,		// モデル用
		kParticle,	// パーティクル用
	};

	enum class RootParameter : uint32_t {
		kWorldTransform, // ワールド変換行列
		kViewProjection, // ビュープロジェクション変換行列
		kMaterial,       // マテリアル
		kTexture,        // テクスチャ
		kLight,          // ライト
	};

	enum class BlendMode : uint32_t {
		kNone,		// ブレンド無し
		kNormal,	// αブレンド
		kAdd,		// 加算合成
		kSubtract,	// 減算合成
		kMultily,	// 乗算合成
		kScreen,	// スクリーン合成

		kTotal	// 総数
	};
private:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// モデル用パイプライン
	static std::array<std::array<ComPtr<ID3D12PipelineState>, 6u>, 2u> graphicsPipelineState_;
	static std::array<ComPtr<ID3D12RootSignature>, 2u> rootSignature_;
	static PipelineType sPipelineType_;

	static std::array<std::array<PipelineState, 6u>, 2u> graphicsPipelineStateClass_;
	static std::array<RootSignature, 2u> rootSignatureClass_;

	static void CreatePipeLine();
	static void BuildPileLine(PipelineType type, D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc);

public:
	static void StaticInit();

	Model() = default;
	~Model() = default;

	std::string name_;
	std::vector<std::unique_ptr<Mesh>> meshList_;
	std::unordered_map<std::string, std::unique_ptr<Material>> materialMap_;

	void Draw(const Transform &transform, const Camera<Render::CameraType::Projecction> &camera) const;
	void Draw(const Transform &transform, const Camera<Render::CameraType::Projecction> &camera, const Material &material) const;
	void Draw(const D3D12_GPU_DESCRIPTOR_HANDLE &transformSRV, uint32_t drawCount, const Camera<Render::CameraType::Projecction> &camera) const;
	template <typename T>
	void Draw(const StructuredBuffer<T> &structurdBuffer, const Camera<Render::CameraType::Projecction> &camera) const;

	static void StartDraw(ID3D12GraphicsCommandList *const commandList);
	static void EndDraw();
	static const char *const defaultDirectory;

	void ImGuiWidget();

	static void SetPipelineType(const PipelineType pipelineType);

	[[nodiscard]] static Model *CreatePlane();

	[[nodiscard]] static Model *const CreateSphere();

	[[nodiscard]] static Model *const LoadObjFile(const std::string &directoryPath, const std::string &fileName);
private:
	void LoadMtlFile(const std::string &directoryPath, const std::string &fileName);

	static ID3D12GraphicsCommandList *commandList_;

};



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

	Model::BlendMode blendMode_ = Model::BlendMode::kNone;
	std::string name_;			// マテリアル名
	std::string textureName_;
	uint32_t texHandle_ = 1u;

	CBuffer<MaterialData> materialBuff_;

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

	VertexBuffer<VertexData> vertexBuffer_;

	std::list<VertexData> vertices_;
	std::list<uint32_t> indexs_;

	Material *material_;

	std::unordered_map<size_t, uint32_t> indexMap_;	// 頂点追加用一時データ

	void CreateBuffer();

	void AddVertex(const VertexData &vertex);

	void SetMaterial(Material *const material);
	Material *const GetMaterial() const { return material_; }

	void Draw(ID3D12GraphicsCommandList *const commandList, uint32_t drawCount = 1u, const Material *const material = nullptr) const;


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


class MinecraftModel {

	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	struct Face {
		Face() = default;
		Face(const Face &);
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
		Cube() = default;
		Cube(const Cube &);
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

		//void UpdateMatrix();

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

template<typename T>
inline void Model::Draw(const StructuredBuffer<T> &structurdBuffer, const Camera<Render::CameraType::Projecction> &camera) const {
	Model::Draw(structurdBuffer.GetHeapRange().GetHandle(0u).gpuHandle_, structurdBuffer.size(), camera);
}
