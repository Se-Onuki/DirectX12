#pragma once
#include "../../Utils/Containers/Singleton.h"
#include "../../ResourceObject/ResourceObject.h"
#include "../../ResourceObject/ResourceObjectManager.h"

namespace SolEngine {

	class FullScreenTextureStrage : public SoLib::Singleton<FullScreenTextureStrage> {
		friend SoLib::Singleton<FullScreenTextureStrage>;

		FullScreenTextureStrage() = default;
		FullScreenTextureStrage(const FullScreenTextureStrage &) = delete;
		FullScreenTextureStrage &operator=(const FullScreenTextureStrage &) = delete;
		~FullScreenTextureStrage() = default;

	public:

		inline static const uint32_t kTextureCount_ = 8u;



	};

}