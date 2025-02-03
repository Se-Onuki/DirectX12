/// @file ModelRender.h
/// @brief モデルの描画クラス
/// @author ONUKI seiya
#pragma once
#include "../../Engine/Utils/Containers/Singleton.h"
#include "../../ResourceObject/ResourceObjectManager.h"
#include "ModelData.h"
#include "../../Engine/Utils/Math/Transform.h"
#include "../Base/RootSignature.h"
#include "../Base/PipelineState.h"

namespace SolEngine {

	class ModelRender : public SoLib::Singleton<ModelRender> {
		friend SoLib::Singleton<ModelRender>;
		ModelRender() = default;
		ModelRender(const ModelRender &) = delete;
		ModelRender &operator=(const ModelRender &) = delete;

		template <SoLib::IsRealType T>
		using ResourceManager = ResourceObjectManager<T>;

		using ModelManager = ResourceObjectManager<ModelData>;
		using MeshManager = ResourceObjectManager<Mesh>;
		using MaterialManager = ResourceObjectManager<Material>;

	public:

		/// @brief 初期化
		void Init();
		/// @brief 破棄
		void clear();
		/// @brief データの追加
		/// @param[in] model モデル
		/// @param[in] transform ワールド行列
		void AddData(ModelManager::Handle model, const SoLib::Transform &transform);

		/// @brief ソート
		void Sort();

		/// @brief 描画
		/// @param[in] camera カメラ
		void Draw(const Camera3D &camera) const;

	private:
		::DirectXCommon *dxCommon_;
		ID3D12GraphicsCommandList *commandList_;

	private:

		ResourceHandle<RootSignature> rootSignature_;

		std::array<ResourceHandle<PipelineState>, static_cast<uint32_t>(Model::BlendMode::kTotal)> pipeline_;

		std::array<std::unordered_map<const SoLib::Transform *, std::vector<MeshManager::Handle>>, static_cast<uint32_t>(Model::BlendMode::kTotal)> modelsBuffer_;

	};

}