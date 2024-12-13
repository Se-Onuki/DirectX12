/// @file AlignasWrapper.h
/// @brief アライメントを行うラッパークラス
/// @author ONUKI seiya
#pragma once
#include "../SoLib/SoLib_Traits.h"
#include <array>
template <SoLib::IsRealType T, size_t alignSize = 16>
class alignas(alignSize) AlignasWrapper : public T {
public:
	static_assert(static_cast<int64_t>(alignSize) - sizeof(T) >= 0, "アライメントの値が不正です");
	//using T::T;

	T &operator=(const T &r) { return static_cast<T &>(*this) = r; }
	T &operator*() { return static_cast<T &>(*this); }
	const T &operator*() const { return static_cast<const T &>(*this); }

	std::array<uint8_t, alignSize - sizeof(T)> pad_;
private:
};

// 特殊化: alignSize - sizeof(T) == 0の場合
template <SoLib::IsRealType T, size_t alignSize>
	requires (alignSize - sizeof(T) == 0)
class alignas(alignSize) AlignasWrapper<T, alignSize> : public T {
public:

	T &operator=(const T &r) { return static_cast<T &>(*this) = r; }
	T &operator*() { return static_cast<T &>(*this); }
	const T &operator*() const { return static_cast<const T &>(*this); }

};

//// ヘルパークラス: alignSize - sizeof(T) != 0の場合
//template <SoLib::IsRealType T, size_t alignSize>
//	requires (alignSize - sizeof(T) != 0)
//class alignas(alignSize) AlignasWrapper<T, alignSize> : public T {
//public:
//	using T::T;
//
//	T &operator=(const T &r) {
//		return static_cast<T &>(*this) = r;
//	}
//
//private:
//	std::array<uint8_t, alignSize - sizeof(T)> pad_;
//};