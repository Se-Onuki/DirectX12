#pragma once
#include "../../Utils/Containers/Singleton.h"

namespace SolEngine {

	class ModelRender : public SoLib::Singleton<ModelRender> {
		friend SoLib::Singleton<ModelRender>;
		ModelRender() = default;
		ModelRender(const ModelRender &) = delete;
		ModelRender &operator=(const ModelRender &) = delete;

	public:





	private:



	};

}