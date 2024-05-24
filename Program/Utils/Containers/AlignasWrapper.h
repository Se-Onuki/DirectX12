#pragma once
#include "../SoLib/SoLib_Traits.h"
#include <array>
template <SoLib::IsRealType T, size_t alignSize = 16>
class alignas(alignSize) AlignasWrapper : public T {
public:
	using T::T;

	T &operator=(const T &r) { return static_cast<T &>(*this) = r; }

private:
	std::array<uint8_t, alignSize - sizeof(T)> pad_;
};