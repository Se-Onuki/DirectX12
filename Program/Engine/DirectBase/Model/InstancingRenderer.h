#pragma once
#include "../../Utils/Containers/Singleton.h"
#include "Material.h"
#include "../Base/StructBuffer.h"

namespace SolEngine {

	class InstancingRenderer : public SoLib::Singleton<InstancingRenderer> {

		InstancingRenderer() = default;
		InstancingRenderer(const InstancingRenderer &) = delete;
		InstancingRenderer &operator=(const InstancingRenderer &) = delete;
		~InstancingRenderer() = default;

		friend SoLib::Singleton<InstancingRenderer>;

	public:

		ArrayBuffer<Material> material_;



	};

}