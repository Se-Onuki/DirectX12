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
	};

	template <typename T>
	concept IsResourceSource = requires(T a) {
		//{ a.CreateObject() } -> std::same_as<std::unique_ptr<typename T::Resource>>;
	};

	template <IsResourceObject T>
	class IResourceCreater {
		
	};


}