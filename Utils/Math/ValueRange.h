#pragma once
#include <algorithm>
#include "Random.h"
#include "Vector2.h"

template <typename T>
struct ValueRange {
	T min_ = 0.f;
	T max_ = 0.f;
	using baseType = T;

	T Clamp(T value) const;

	// 使用するランダム関数の変更
	template <typename U>
	T Random() const;

	/// @ 
	T Random() const;
};


template <typename T>
T ValueRange<T>::Clamp(T value) const {
	return std::clamp<T>(value, min_, max_);
}

template <typename T>
template <typename U>
T ValueRange<T>::Random() const {
	return Random::GetRandom<U>(min_, max_);
}
template <typename T>
T ValueRange<T>::Random() const {
	return Random::GetRandom(min_, max_);
}
