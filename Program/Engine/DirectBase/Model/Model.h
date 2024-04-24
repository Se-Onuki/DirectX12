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
#include <memory>

#include "../../../Utils/Math/Transform.h"
#include "../Base/VertexBuffer.h"
#include "../Base/RootSignature.h"
#include "../Base/PipelineState.h"

#include "../Base/StructBuffer.h"
#include "../Descriptor/DescriptorManager.h"
#include "../../Utils/Graphics/Color.h"
#include "../../Utils/Text/StaticString.h"
#include <assimp/mesh.h>

#include "../../Utils/SoLib/SoLib_Timer.h"
#include "../../Utils/SoLib/SoLib_Traits.h"

class ViewProjection;

struct Material;
struct Mesh;

struct ModelNode {
	/// @brief データを解析してノードを作成する
	/// @param node assimpのノード
	/// @return 解析結果
	static ModelNode Create(aiNode *node);
	ModelNode() = default;

	static std::unique_ptr<CBuffer<Matrix4x4>> kIdentity_;

	const CBuffer<Matrix4x4> &GetLocalMatrix() const;

	// 回転の姿勢
	std::unique_ptr<CBuffer<Matrix4x4>> localMatrix_;
	// ノード名
	std::string name_;
	// 子供ノード
	std::vector<ModelNode> children_;

};

namespace Animation {

	struct IKeyFlame {};

	template<SoLib::IsRealType T>
	struct KeyFlameTemplate : IKeyFlame {
		T value_;					// キーフレームの時の値
		SoLib::Time::SecondF time_;	// キーフレームの時刻
	};

	/// @brief Vector3のキーフレーム
	using KeyFlameVector3 = KeyFlameTemplate<Vector3>;

	/// @brief Quaternionのキーフレーム
	using KeyFlameQuaternion = KeyFlameTemplate<Quaternion>;

	template<typename T>
	concept KeyFlameTypes = SoLib::IsBased<T, IKeyFlame>;

	template<KeyFlameTypes T>
	struct AnimationCurve {
		std::vector<T> keyFlames_;
		auto begin() { return keyFlames_.begin(); }
		auto begin() const { return keyFlames_.begin(); }
		auto cbegin() const { return keyFlames_.cbegin(); }

		auto end() { return keyFlames_.end(); }
		auto end() const { return keyFlames_.end(); }
		auto cend() const { return keyFlames_.cend(); }

		size_t size() const { return keyFlames_.size(); }

		T &at(size_t i) { return keyFlames_.at(i); }
		const T &at(size_t i) const { return keyFlames_.at(i); }

		T &operator[](size_t i) { return keyFlames_[i]; }
		const T &operator[](size_t i) const { return keyFlames_[i]; }

		T *data() { return keyFlames_.data(); }
		const T *data() const { return keyFlames_.data(); }
	};

	// ノードごとのアニメーション ( AnimationCurve )
	struct NodeAnimation {

		AnimationCurve<KeyFlameVector3> scale_;		// スケール要素のAnimationCurve
		AnimationCurve<KeyFlameQuaternion> rotate_;	// 回転要素のAnimationCurve
		AnimationCurve<KeyFlameVector3> translate_;	// 平行移動要素のAnimationCurve
	};

	struct Animaiton {
		SoLib::Time::SecondF duration_;							// アニメーション全体の尺
		std::map<std::string, NodeAnimation> nodeAnimaitons_;	// NodeAnimationの集合｡
	};

}
class Model {
public:
	enum class PipelineType : uint32_t {
		kModel,		// モデル用
		kParticle,	// パーティクル用
		kShadowParticle,	// パーティクル用
	};

	enum class RootParameter : uint32_t {
		kWorldTransform,	// ワールド変換行列
		kViewProjection,	// ビュープロジェクション変換行列
		kMaterial,			// マテリアル
		kTexture,			// テクスチャ
		kLight,				// ライト
		kInstanceLocation,	// インスタンス先頭値
		kModelTransform,	// モデルの回転情報

		kSize,				// enumのサイズ
	};

	enum class BlendMode : uint32_t {
		kNone,		// ブレンド無し
		kNormal,	// αブレンド
		kAdd,		// 加算合成
		kSubtract,	// 減算合成
		kMultily,	// 乗算合成
		kScreen,	// スクリーン合成

		kBacker,	// 後ろならば
		kAlways,	// 常に

		kTotal	// 総数
	};
private:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// モデル用パイプライン
	static std::array<std::array<ComPtr<ID3D12PipelineState>, 8u>, 3u> graphicsPipelineState_;
	//static std::array<ComPtr<ID3D12RootSignature>, 2u> rootSignature_;
	static PipelineType sPipelineType_;

	//static std::array<std::array<PipelineState, 8u>, 2u> graphicsPipelineStateClass_;
	static std::array<RootSignature, 2u> rootSignatureClass_;

	static void CreatePipeLine();
	static void BuildPipeLine(PipelineType type, D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc);

public:
	static void StaticInit();

	Model() = default;
	~Model() = default;

	std::string name_;
	std::vector<std::unique_ptr<Mesh>> meshList_;
	std::unordered_map<std::string, std::unique_ptr<Material>> materialMap_;

	ModelNode rootNode_;

	void Draw(const Transform &transform, const Camera3D &camera) const;
	void Draw(const Transform &transform, const Camera3D &camera, const Material &material) const;
	void Draw(const D3D12_GPU_DESCRIPTOR_HANDLE &transformSRV, uint32_t drawCount, const CBuffer<uint32_t> &drawIndex, const Camera3D &camera) const;
	void Draw(const D3D12_GPU_DESCRIPTOR_HANDLE &transformSRV, uint32_t drawCount, const CBuffer<uint32_t> &drawIndex, const CBuffer<Camera3D::CameraMatrix> &camera) const;
	template <typename T>
	void Draw(const StructuredBuffer<T> &structurdBuffer, const Camera3D &camera) const;

	static void StartDraw(ID3D12GraphicsCommandList *const commandList);
	static void EndDraw();

	/// @brief "resources/"
	using DefaultDirectory = SoLib::Text::StaticString<"resources/">;

	bool ImGuiWidget();

	static void SetPipelineType(const PipelineType pipelineType);

	[[nodiscard]] static std::unique_ptr<Model> CreatePlane();

	[[nodiscard]] static std::unique_ptr<Model> CreateSphere();

	[[nodiscard]] static std::unique_ptr<Model> LoadObjFile(const std::string &directoryPath, const std::string &fileName);

	[[nodiscard]] static std::unique_ptr<Model> LoadAssimpModelFile(const std::string &directoryPath, const std::string &fileName);
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
		SoLib::Color::RGB4 color;			// 色(RGBA)
		Vector4 emissive;		// 自己発光色(RGBA)
		Matrix4x4 uvTransform;

		float shininess = { 1.f };
	};

	Model::BlendMode blendMode_ = Model::BlendMode::kNone;
	std::string name_;			// マテリアル名
	std::string textureName_;
	uint32_t texHandle_ = 1u;

	CBuffer<MaterialData> materialBuff_;

	void CreateBuffer();

	bool ImGuiWidget();

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
	struct hashVertex {
		size_t operator()(const Mesh::VertexData &v) const {
			std::string s =
				std::to_string(v.position.x) + "/" + std::to_string(v.position.y) + "/" + std::to_string(v.position.z) + "/" + std::to_string(v.position.w)	// 頂点
				+ "/" + std::to_string(v.texCoord.x) + "/" + std::to_string(v.texCoord.y)	// uv座標
				+ "/" + std::to_string(v.normal.x) + "/" + std::to_string(v.normal.y) + "/" + std::to_string(v.normal.z);	// 法線
			return std::hash<std::string>()(s);
		}
	};

	VertexBuffer<VertexData> vertexBuffer_;

	std::list<VertexData> vertices_;
	std::list<uint32_t> indexs_;

	Material *material_;

	std::unordered_map<VertexData, uint32_t, hashVertex> indexMap_;	// 頂点追加用一時データ

	void CreateBuffer();

	void AddVertex(const VertexData &vertex);

	void SetMaterial(Material *const material);
	Material *const GetMaterial() const { return material_; }

	void Draw(ID3D12GraphicsCommandList *const commandList, uint32_t drawCount = 1u, const Material *const material = nullptr) const;


};



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
	Model::Draw(structurdBuffer.GetHeapRange().GetHandle(0u).gpuHandle_, structurdBuffer.size(), structurdBuffer.GetStartIndex(), camera);
}
