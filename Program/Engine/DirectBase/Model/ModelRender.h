#pragma once
#include "../../Utils/Containers/Singleton.h"
#include "../../ResourceObject/ResourceObjectManager.h"
#include "ModelData.h"
#include "../../Utils/Math/Transform.h"

namespace SolEngine {

	class ModelRender : public SoLib::Singleton<ModelRender> {
		friend SoLib::Singleton<ModelRender>;
		ModelRender() = default;
		ModelRender(const ModelRender &) = delete;
		ModelRender &operator=(const ModelRender &) = delete;

		template <SoLib::IsRealType T>
		using ResourceManager = ResourceObjectManager<T>;

	public:


		void Init();

		void SetData();


		void Draw(ResourceManager<ModelData>::Handle model, const Transform &transform) const;

	private:

	};

}