/// @file Model.h
/// @brief モデルの実装(旧モデル)
/// @author ONUKI seiya
#pragma once
#include "../Render/Camera.h"
#include <vector>

#include <fstream>
#include <sstream>
#include <string>

#include <list>
#include <unordered_map>

#include "../../Engine/Utils/Math/Matrix4x4.h"
#include "../../Engine/Utils/Math/Vector2.h"
#include "../../Engine/Utils/Math/Vector3.h"
#include "../../Engine/Utils/Math/Vector4.h"
#include "../../Engine/Utils/Math/Math.hpp"

#include <d3d12.h>
#include <wrl.h>

#include <array>
#include <memory>

#include "../../Engine/Utils/Math/Transform.h"
#include "../Base/PipelineState.h"
#include "../Base/RootSignature.h"
#include "../Base/VertexBuffer.h"

#include "../../Engine/Utils/Graphics/Color.h"
#include "../../Engine/Utils/Text/StaticString.h"
#include "../Base/StructBuffer.h"
#include "../Descriptor/DescriptorManager.h"
#include <assimp/mesh.h>

#include "../../Engine/Utils/SoLib/SoLib_Timer.h"
#include "../../Engine/Utils/SoLib/SoLib_Traits.h"
#include "../../Engine/Utils/Containers/Singleton.h"

#include "../Base/EngineObject.h"
#include <optional>

class ViewProjection;

struct Material;
struct Mesh;
class Model;

namespace SolEngine {
	class ModelData;
	class AssimpData;
	class SkeletonJointReference;
	class SkyBox;
	namespace SkeletonAnimation {
		class Animation;
		class Skeleton;
	}
}

struct ModelNode {
	/// @brief データを解析してノードを作成する
	/// @param node assimpのノード
	/// @return 解析結果
	static std::unique_ptr<ModelNode> Create(aiNode *node);
	ModelNode() = default;

	static std::unique_ptr<CBuffer<Matrix4x4>> kIdentity_;

	/// @brief ローカル行列を取得
	/// @return ローカル行列
	const CBuffer<Matrix4x4> &GetLocalMatrix() const;

	/// @brief アフィン変換の計算を行う
	void CalcAffine();

	// ノードの状態
	SoLib::SimpleTransformQuaternion transform_;

	// 回転の姿勢
	std::unique_ptr<CBuffer<Matrix4x4>> localMatrix_;
	// ノード名
	std::string name_;
	// 子供ノード
	std::vector<std::unique_ptr<ModelNode>> children_;

	/// @brief 名前の一致したノードを返す
	/// @param[in] name 名前
	/// @return ノード
	const ModelNode *FindNode(const std::string &name) const;
};

struct ModelJointReference {
	/// @brief 再帰的に子ノードを構築する
	/// @param[in] node ノード
	/// @param[in] parent 親のノードのIndex
	/// @param[out] joints 出力先
	/// @return 自分自身のIndex
	static uint32_t MakeJointIndex(const ModelNode *node, const std::optional<uint32_t> parent, std::vector<std::unique_ptr<ModelJointReference>> &joints);

	// 名前
	std::string name_;
	// 子jointへのIndexリスト
	std::vector<uint32_t> children_;
	// 自分自身のIndex
	uint32_t index_;
	// 自身の親のIndex。存在しない場合はnullopt
	std::optional<uint32_t> parent_ = std::nullopt;

};

struct ModelJointState {

	/// @brief 再帰的に子ノードを構築する
	/// @param[in] node ノード
	/// @param[out] joints 出力先
	/// @return 自分自身のIndex
	static uint32_t MakeJointIndex(const ModelNode *node, std::vector<std::unique_ptr<ModelJointState>> &joints);

	/// @brief アフィン変換の計算を行う
	inline void CalcAffine() { localMatrix_ = transform_.Affine(); }

	// transform情報
	SoLib::SimpleTransformQuaternion transform_;
	// ローカルの体勢情報
	Matrix4x4 localMatrix_;
	// スケルトン空間での変換行列
	Matrix4x4 skeletonSpaceMatrix_;

};

struct SkeletonState {

	/// @brief スケルトンを構築する
	/// @param[in] skeleton スケルトンの元のデータ
	/// @return 構築されたスケルトン
	static std::unique_ptr<SkeletonState> MakeSkeleton(const SolEngine::SkeletonAnimation::Skeleton *skeleton);

	/// @brief Matrixを更新する
	void UpdateMatrix();

	/// @brief アニメーションを適用する
	/// @param[in] animation アニメーションのデータ
	/// @param[in] animateTime アニメーションの時間
	void ApplyAnimation(const SolEngine::SkeletonAnimation::Animation &animation, const float animateTime);

	// 所属しているJointのデータ
	std::vector<ModelJointState> joints_;

	// データの参照
	const SolEngine::SkeletonJointReference *reference_ = nullptr;

	/// @brief スケルトンの状態を描画する
	/// @param[in] transMat 行列
	/// @param[in] drawOffset 描画オフセット
	void AddDrawBuffer(const Matrix4x4 &transMat, const Vector3 &drawOffset = {}) const;

};


template<size_t InfluenceCount = 1u>
struct VertexWeightData {
	// 重み係数
	std::conditional_t<InfluenceCount != 1, std::array<float, InfluenceCount>, float> weight_;
	// 頂点Index
	std::conditional_t<InfluenceCount != 1, std::array<uint32_t, InfluenceCount>, uint32_t> vertexIndex_;
};

struct JointWeightData {
	// 関節空間の変換行列
	Matrix4x4 inverseBindPoseMatrix_;
	// 所属する頂点の重み情報
	std::vector<VertexWeightData<>> vertexWeightData_;
};

struct SkinClusterBaseData {
	std::unordered_map<std::string, JointWeightData> skinClusterData_;
};

struct VertexInfluence {
	static constexpr uint32_t kNumMaxInfluence_ = 4u;
	VertexWeightData<kNumMaxInfluence_> vertexInfluence_;
};

struct WellForGPU {
	Matrix4x4 skeletonSpaceMatrix; // 位置用
	Matrix4x4 skeletonSpaceInverseTransponeMatrix; // 法線用
};

struct SkinCluster {
	/// @brief コンストラクタ
	/// @param[in] jointsCount スケルトンのJoint数
	/// @details 指定したサイズでバッファを作成する
	SkinCluster(uint32_t jointsCount);
	/// @brief スキンクラスターを構築する
	/// @param[in] model モデルのデータ
	/// @param[in] skeletonRef スケルトンの参考データ
	/// @param[in] skeleton スケルトンの状態
	/// @return 構築されたスキンクラスター
	static std::unique_ptr<SkinCluster> MakeSkinCluster(const SolEngine::ModelData *model, const SolEngine::SkeletonAnimation::Skeleton *skeletonRef, const SkeletonState &skeleton);

	/// @brief スキンクラスターを更新する
	/// @param[in] skeleton スケルトンの状態
	void Update(const SkeletonState &skeleton);

	std::vector<Matrix4x4> inverseBindPoseMatrixList_;

	std::span<WellForGPU> paletteSpan_;

	const StructuredBuffer<WellForGPU> &GetPalette() const { return palette_; }

	const SolEngine::ModelData *reference_ = nullptr;
	const SolEngine::SkeletonAnimation::Skeleton *skeletonRef_ = nullptr;

private:

	StructuredBuffer<WellForGPU> palette_;
};

struct SkinModel {
	/// @brief スキンモデルを構築する
	/// @param[in] model モデルのデータ
	/// @param[in] skeleton スケルトン
	/// @return 構築されたスキンモデル
	/// @details スキンクラスターとモデルの紐づけを行い、スキンモデルを構築する
	static std::unique_ptr<SkinModel> MakeSkinModel(SolEngine::ModelData *model, SolEngine::SkeletonAnimation::Skeleton *skeleton);
	void Update(const SolEngine::SkeletonAnimation::Animation &animation, const float animateTime);

	std::unique_ptr<SkinCluster> skinCluster_ = nullptr;
	std::unique_ptr<SkeletonState> skeleton_ = nullptr;

};

class Model {
public:
	enum class PipelineType : uint32_t {
		kModel,          // モデル用
		kParticle,       // パーティクル用
		kSkinModel,          // モデル用
		kSkinParticle, // パーティクル用
		kShadowParticle, // パーティクル用

		kSize,
	};

	enum class RootParameter : uint32_t {
		kWorldTransform,	// ワールド変換行列
		kViewProjection,	// ビュープロジェクション変換行列
		kMaterial,			// マテリアル
		kTexture,			// テクスチャ
		kLight,				// ライト
		kInstanceLocation,	// インスタンス先頭値
		kMatrixPalette,		// スキン行列の配列
		kModelTransform,   // モデルの回転情報
		kSkyBox,

		kSize, // enumのサイズ
	};

	enum class BlendMode : uint32_t {
		kNone,     // ブレンド無し
		kNormal,   // αブレンド
		kAdd,      // 加算合成
		kSubtract, // 減算合成
		kMultily,  // 乗算合成
		kScreen,   // スクリーン合成

		kBacker, // 後ろならば
		kAlways, // 常に

		kTotal // 総数
	};

private:
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	// モデル用パイプライン
	static std::array<std::array<ComPtr<ID3D12PipelineState>, 8u>, static_cast<uint32_t>(PipelineType::kSize)> graphicsPipelineState_;
	// static std::array<ComPtr<ID3D12RootSignature>, 2u> rootSignature_;
	static PipelineType sPipelineType_;

	// static std::array<std::array<PipelineState, 8u>, 2u> graphicsPipelineStateClass_;
	static std::array<RootSignature, 2u> rootSignatureClass_;

	/// @brief すべてのパイプラインを構築する
	static void CreatePipeLine();
	/// @brief パイプラインを構築する
	/// @param[in] type パイプラインの種類
	/// @param[in] pipelineDesc パイプラインの設定
	/// @details ブレンドモードに対応したパイプラインを構築する
	static void BuildPipeLine(PipelineType type, D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc);

	/// @brief モデルのヒープのサイズ
	static constexpr uint32_t kModelHeapCount_ = 32u;


public:
	static void StaticInit();

	Model() = default;
	~Model() = default;

	std::string name_;
	std::vector<std::unique_ptr<Mesh>> meshList_;
	std::unordered_map<std::string, std::unique_ptr<Material>> materialMap_;

	std::unique_ptr<ModelNode> rootNode_;

	SkinClusterBaseData skinCluster_;

	/// @brief モデルを描画する
	void Draw(const SoLib::Transform &transform, const SolEngine::Camera3D &camera) const;
	void Draw(const SkinCluster &skinCluster, const SoLib::Transform &transform, const SolEngine::Camera3D &camera) const;
	//void Draw(const Transform &transform, const Camera3D &camera, const Material &material) const;
	void Draw(const SkinCluster &skinCluster, const D3D12_GPU_DESCRIPTOR_HANDLE &transformSRV, uint32_t drawCount, const CBuffer<uint32_t> &drawIndex, const SolEngine::Camera3D &camera) const;
	void Draw(const D3D12_GPU_DESCRIPTOR_HANDLE &transformSRV, uint32_t drawCount, const CBuffer<uint32_t> &drawIndex, const SolEngine::Camera3D &camera) const;
	void Draw(const D3D12_GPU_DESCRIPTOR_HANDLE &transformSRV, uint32_t drawCount, const CBuffer<uint32_t> &drawIndex, const CBuffer<SolEngine::Camera3D::CameraMatrix> &camera) const;
	template <typename T>
	void Draw(const StructuredBuffer<T> &structurdBuffer, const SolEngine::Camera3D &camera) const;

	template <typename T>
	void Draw(const SkinCluster &skinCluster, const StructuredBuffer<T> &structurdBuffer, const SolEngine::Camera3D &camera) const;

	/// @brief スカイボックスを設定する
	static void SetSkyBox(const SolEngine::SkyBox &skyBox);

	/// @brief 描画を開始する
	/// @param[in] commandList コマンドリスト
	/// @details commandListを取得し､描画の前準備を行う
	static void StartDraw(ID3D12GraphicsCommandList *const commandList);
	/// @brief 描画を終了する
	/// @details commandListをnullptrにする
	static void EndDraw();

	/// @brief "resources/"
	using DefaultDirectory = SoLib::Text::StaticString<"resources/">;

	/// @brief ImGuiにウィジェットを表示する
	/// @return 変更があったらtrue
	bool ImGuiWidget();

	/// @brief パイプラインの種類を設定する
	/// @param[in] pipelineType パイプラインの種類
	static void SetPipelineType(const PipelineType pipelineType);
	/// @brief パイプラインの種類を取得する
	/// @return パイプラインの種類
	static const PipelineType GetPipelineType() { return sPipelineType_; };

	/// @brief GraphicsPipelineStateを取得する
	/// @return GraphicsPipelineStateの配列
	static const auto &GetGraphicsPipelineState() { return graphicsPipelineState_; }

	/// @brief 平面モデルを作成する
	/// @return 作成したモデル
	[[nodiscard]] static std::unique_ptr<Model> CreatePlane();

	/// @brief 球モデルを作成する
	/// @return 作成したモデル
	[[nodiscard]] static std::unique_ptr<Model> CreateSphere();

	/// @brief OBJファイルを読み込む
	/// @param[in] directoryPath ディレクトリパス
	/// @param[in] fileName ファイル名
	/// @return 読み込んだモデル
	[[nodiscard]] static std::unique_ptr<Model> LoadObjFile(const std::string &directoryPath, const std::string &fileName);

	/// @brief Assimpファイルを読み込む
	/// @param[in] directoryPath ディレクトリパス
	/// @param[in] fileName ファイル名
	/// @return 読み込んだモデル
	[[nodiscard]] static std::unique_ptr<Model> LoadAssimpModelFile(const std::string &directoryPath, const std::string &fileName);

private:
	/// @brief MTLファイルを読み込む
	/// @param[in] directoryPath ディレクトリパス
	/// @param[in] fileName ファイル名
	void LoadMtlFile(const std::string &directoryPath, const std::string &fileName);

	static ID3D12GraphicsCommandList *commandList_;
};

struct Material {

private:
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	Material() = default;
	Material(const std::string &materialName)
	{
		name_ = materialName;
	}

	/// @struct MaterialData
	/// @brief マテリアルデータ
	struct MaterialData {
		SoLib::Color::RGB4 color; // 色(RGBA)
		Vector4 emissive;         // 自己発光色(RGBA)
		Vector4 ambient;         // 自己発光色(RGBA)
		Matrix4x4 uvTransform;

		float shininess = { 1.f };
		float shininessStrength = { 1.f };
	};

	Model::BlendMode blendMode_ = Model::BlendMode::kNone;
	std::string name_; // マテリアル名
	std::string textureName_;
	uint32_t texHandle_ = 1u;

	CBuffer<MaterialData> materialBuff_;

	/// @brief バッファの作成
	void CreateBuffer();

	/// @brief ImGuiにウィジェットを表示する
	/// @return 変更があったらtrue
	bool ImGuiWidget();

	/// @brief マテリアルを作成する
	void Create();
};

struct Mesh {
private:
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	struct VertexData {
		Vector4 position; // 頂点位置
		Vector3 normal;   // 法線
		Vector2 texCoord; // UV座標系

		// 比較。すべてが一致した場合のみ真を返す
		bool operator==(const VertexData &vertex) const
		{
			return position == vertex.position && texCoord == vertex.texCoord && normal == vertex.normal;
		}
	};
	struct hashVertex {
		size_t operator()(const Mesh::VertexData &v) const
		{
			std::string s =
				std::to_string(v.position.x) + "/" + std::to_string(v.position.y) + "/" + std::to_string(v.position.z) + "/" + std::to_string(v.position.w) // 頂点
				+ "/" + std::to_string(v.texCoord.x) + "/" + std::to_string(v.texCoord.y)                                                                   // uv座標
				+ "/" + std::to_string(v.normal.x) + "/" + std::to_string(v.normal.y) + "/" + std::to_string(v.normal.z);                                   // 法線
			return std::hash<std::string>()(s);
		}
	};

	VertexBuffer<VertexData> vertexBuffer_;
	IndexBuffer<uint32_t> indexBuffer_;

	std::list<VertexData> vertices_;
	std::list<uint32_t> indexs_;

	Material *material_ = nullptr;

	const ModelNode *pNode_ = nullptr;

	std::string meshName_;

	std::unordered_map<VertexData, uint32_t, hashVertex> indexMap_; // 頂点追加用一時データ

	/// バッファの作成
	void CreateBuffer();

	/// 頂点の追加
	void AddVertex(const VertexData &vertex);

	/// @brief マテリアルを設定
	void SetMaterial(Material *const material);
	/// @brief マテリアルを取得
	/// @return マテリアル
	Material *const GetMaterial() const { return material_; }

	/// @brief 描画
	/// @param[in] commandList コマンドリスト
	/// @param[in] drawCount 描画する数
	/// @param[in] vbv 頂点データのビュー
	/// @param[in] vertexOffset 頂点オフセット
	void Draw(ID3D12GraphicsCommandList *const commandList, uint32_t drawCount = 1u, const D3D12_VERTEX_BUFFER_VIEW *vbv = nullptr, uint32_t vertexOffset = 0u) const;
};

struct MeshFactory {

	std::list<Mesh::VertexData> vertices_;
	std::list<uint32_t> indexs_;

	Material *pMaterial_ = nullptr;

	const ModelNode *pNode_ = nullptr;

	std::string meshName_;

	std::unordered_map<Mesh::VertexData, uint32_t, Mesh::hashVertex> indexMap_; // 頂点追加用一時データ

	/// @brief メッシュを作成
	/// @return 作成したメッシュ
	/// @details 保存されているデータからメッシュを作成する
	std::unique_ptr<Mesh> CreateMesh() const;

};

class MinecraftModel {

	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	struct Face {
		Face() = default;
		Face(const Face &);
		D3D12_VERTEX_BUFFER_VIEW vbView_ = {};
		D3D12_INDEX_BUFFER_VIEW ibView_ = {};

		ComPtr<ID3D12Resource> vertexBuff_;
		ComPtr<ID3D12Resource> indexBuff_;

		Mesh::VertexData *vertices_ = nullptr;
		uint32_t *indexs_ = nullptr;

		/// @brief バッファの作成
		void CreateBuffer();
		/// @brief 初期化全般
		void Init();
		/// 頂点のデータの設定
		/// @param[in] vertex 頂点情報
		/// @param[in] normal 法線
		void SetVertex(const std::array<Vector3, 4u> &vertex, const Vector3 &normal);
		/// @brief 描画
		/// @param[in] commandList コマンドリスト
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
		SoLib::Transform transformLocal_;

		// void UpdateMatrix();

		/// @brief 初期化
		void Init();
		/// @brief 描画
		/// @param[in] commandList コマンドリスト
		void Draw(ID3D12GraphicsCommandList *const commandList);

		/// @brief 頂点の設定
		/// @param[in] origin 原点
		/// @param[in] size Cubeのサイズ
		void SetVertex(const Vector3 &origin, const Vector3 &size);

		/// @brief 行列のリセット
		void ResetTransform();
	};

	struct Bone {
		std::string name_;
		std::vector<Cube> cubes_;
		std::unordered_map<std::string, Bone> children_;

		Bone *parent_ = nullptr;

		SoLib::Transform transform_;

		/// @brief 行列の更新
		void UpdateTransform();
		/// @brief 親の設定
		/// @param[in] parent 親
		void SetParent(Bone *const parent);

		/// @brief 描画
		/// @param[in] commandList コマンドリスト
		void Draw(ID3D12GraphicsCommandList *const commandList);
	};

public:
	Vector2 texelSize_ = {};
	uint32_t textureHandle_ = 1u;

	std::unordered_map<std::string, Bone> bones_ = {};
	std::unordered_map<std::string, Bone> children_{};

	MinecraftModel() = default;
	~MinecraftModel() = default;

	SoLib::Transform transformOrigin_;

	/// @brief 描画
	/// @param[in] commandList コマンドリスト
	void Draw(ID3D12GraphicsCommandList *const commandList);

	/// @brief 読み込み
	/// @param[in] file_path 読み込みファイル
	void LoadJson(const std::string &file_path);
};

template <typename T>
inline void Model::Draw(const StructuredBuffer<T> &structurdBuffer, const SolEngine::Camera3D &camera) const
{
	Model::Draw(structurdBuffer.GetHeapRange().GetHandle(0u).gpuHandle_, structurdBuffer.size(), structurdBuffer.GetStartIndex(), camera);
}

template <typename T>
inline void Model::Draw(const SkinCluster &skinCluster, const StructuredBuffer<T> &structurdBuffer, const SolEngine::Camera3D &camera) const
{
	Model::Draw(skinCluster, structurdBuffer.GetHeapRange().GetHandle(0u).gpuHandle_, structurdBuffer.size(), structurdBuffer.GetStartIndex(), camera);
}
