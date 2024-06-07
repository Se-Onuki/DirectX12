#pragma once
#include "../SoLib/SoLib_Traits.h"

namespace SoLib {

	template <IsRealType T>
	struct TypeID {
		inline static constexpr size_t kValue_ = 0;
		using Type = T;

		TypeID() = default;

		bool operator==(const TypeID &other) const = default;
		constexpr operator size_t() noexcept { return kValue_; }
		constexpr operator size_t() const noexcept { return kValue_; }
	};
}