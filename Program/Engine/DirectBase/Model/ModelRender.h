#pragma once
#include "../../Utils/Containers/Singleton.h"
#include "../../ResourceObject/ResourceObjectManager.h"
#include "ModelData.h"
#include "../../Utils/Math/Transform.h"
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


		void Init();

		void clear();


		void AddData(ModelManager::Handle model, const Transform &transform);

	private:

		ResourceManager<RootSignature>::Handle rootSignature_;

		std::array<ResourceManager<PipelineState>::Handle, static_cast<uint32_t>(Model::BlendMode::kTotal)> pipeline_;

		std::array<std::unordered_map<const Transform *, std::vector<MeshManager::Handle>>, static_cast<uint32_t>(Model::BlendMode::kTotal)> modelsBuffer_;

	};

}