/// @file ValueRange.h
/// @brief 値のペア
/// @author ONUKI seiya
#pragma once
#include <algorithm>
#include "Random.h"
#include "Vector2.h"

template <typename T>
struct ValuePair {
	T min_ = 0.f;
	T max_ = 0.f;
	using baseType = T;

	/// @brief 値のクランプ
	T Clamp(T value) const;

	// 使用するランダム関数の変更
	template <typename U>
	T Random() const;

	/// @brief デフォルトのランダム関数
	T Random() const;
};


template <typename T>
T ValuePair<T>::Clamp(T value) const {
	return std::clamp<T>(value, min_, max_);
}

template <typename T>
template <typename U>
T ValuePair<T>::Random() const {
	return Random::GetRandom<U>(min_, max_);
}
template <typename T>
T ValuePair<T>::Random() const {
	return min_ >= max_ ? min_ : Random::GetRandom(min_, max_);
}
