#pragma once
#include <concepts>
#include "../../Utils/SoLib/SoLib_Traits.h"

namespace SolEngine {
	class IResourceObject {

	public:

		IResourceObject() = default;
	};

	template <typename T>
	concept IsResourceObject = SoLib::IsBased<T, IResourceObject>;

	template <IsResourceObject T>
	class IResourceSource {
		//static_assert(0, "特殊化が行われていないリソースオブジェクトです");
	};

}