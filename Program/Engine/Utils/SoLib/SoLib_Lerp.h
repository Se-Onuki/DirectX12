/// @file SoLib_Lerp.h
/// @brief Lerpの実装
/// @author ONUKI seiya
#pragma once
#include <cstdint>

namespace SoLib {
struct Vector3;

	/// @brief 線形補間
	/// @tparam T 任意の型
	/// @param Start 始点
	/// @param End 終点
	/// @param t 比率
	/// @return 補間した値
	template<typename T>
	inline T Lerp(const T &Start, const T &End, float t) {
		return static_cast<T>(Start + (End - Start) * t);
	}

	template<typename T, typename U>
	inline U Lerp(const T &start, const T &end, U T:: *ptr, const float t) {
		return Lerp((start.*ptr), (end.*ptr), t);
	}

	/// @brief 球面線形補間
	/// @param Start 始点のオイラー角
	/// @param End 終点のオイラー角
	/// @param t 比率
	/// @return 補間したオイラー角
	Vector3 Slerp(const Vector3 &Start, const Vector3 &End, float t);


	/// @brief 色用の線形補間
	/// @param statColor 始点RGBA色
	/// @param endColor 終点RGBA色
	/// @param easingVolume 係数
	/// @return 計算結果
	uint32_t ColorLerp(uint32_t statColor, uint32_t endColor, float easingVolume);

}