#pragma once
#include "../../Engine/Utils/Containers/Singleton.h"
#include "Material.h"
#include "../Base/StructBuffer.h"
#include "ModelData.h"

namespace SolEngine {

	template<typename MonoElem, typename... Ts>
	class InstancingRenderer : public SoLib::Singleton<InstancingRenderer> {

		InstancingRenderer() = default;
		InstancingRenderer(const InstancingRenderer &) = delete;
		InstancingRenderer &operator=(const InstancingRenderer &) = delete;
		~InstancingRenderer() = default;

		friend SoLib::Singleton<InstancingRenderer>;

	public:

		void Init(uint32_t count);

		void Draw(const ModelData *model) const;

		std::tuple<ArrayBuffer<Ts>...> elements_;

	};

}