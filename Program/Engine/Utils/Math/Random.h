/// @file Random.h
/// @brief ランダム関数
/// @author ONUKI seiya
#pragma once
#include <stdint.h>
#include <random>

#include "Vector2.h"

namespace Random {
	// ランダム関数の前方宣言
	template <typename T>
	T GetRandom(T, T);

	/// @brief 整数型ランダム関数
	/// @param min 最小値
	/// @param max 最大値
	/// @return 整数型のランダム値
	template <>
	inline int32_t GetRandom<int32_t>(int32_t min, int32_t max);

	/// @brief 浮動小数のランダム関数
	/// @param min 最小値
	/// @param max 最大値
	/// @return 浮動小数のランダム値
	template <>
	inline float GetRandom<float>(float min, float max);

	/// @brief ベクタ型のランダム関数
	/// @param min 最小値
	/// @param max 最大値
	/// @return ベクタ型のランダム値
	template <typename T = float>
	inline Vector2 GetRandom(const Vector2 min, const Vector2 max);

	/// @brief 浮動小数のランダム関数
	/// @param min 最小値
	/// @param max 最大値
	/// @param count 配列の量
	/// @return 浮動小数のランダム値
	// float GetRandom(float min, float max, uint32_t count);
}

template <>
int32_t Random::GetRandom<int32_t>(int32_t min, int32_t max) {

	if (min >= max) { return min; }

	static std::mt19937 gen;
	static bool init;
	if (!init) {
		std::random_device rd;
		gen.seed(rd());
		init = true;
	}
	std::uniform_int_distribution<int32_t> dist(min, max);
	return dist(gen);
}

template <>
float Random::GetRandom<float>(float min, float max) {

	if (min >= max) { return min; }

	static std::mt19937 gen;
	static bool init;
	if (!init) {
		std::random_device rd;
		gen.seed(rd());
		init = true;
	}
	std::uniform_real_distribution<float> dist(min, max);
	return dist(gen);
}

template <typename T>
Vector2 Random::GetRandom(const Vector2 min, const Vector2 max) {

	// Tに合わせて変換を行い、floatに戻してVector2に戻す
	return Vector2{
		static_cast<float>(Random::GetRandom<T>(static_cast<T>(min.x), static_cast<T>(max.x))),
		static_cast<float>(Random::GetRandom<T>(static_cast<T>(min.y), static_cast<T>(max.y)))
	};
}
