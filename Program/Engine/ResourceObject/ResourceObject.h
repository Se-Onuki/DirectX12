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
	class ResourceSource {
	};

	//template <typename T>
	//concept IsResourceSource =  //requires(T a) {
	//	//{ a.CreateObject() } -> std::same_as<std::unique_ptr<typename T::Resource>>;
	////};

	template <IsResourceObject T, SoLib::IsRealType Source = ResourceSource<T>>
	class ResourceCreater {
		using SourceType = Source;

	};

	template <typename T, typename Resource, typename Source>
	concept IsResourceCreater = requires(const T t, const Source source) {
		{t.CreateObject(source)} -> std::same_as<typename std::unique_ptr<Resource>>;
		//requires std::same_as<typename T::SourceType, Source>;
	};


}