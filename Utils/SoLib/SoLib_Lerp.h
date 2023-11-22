#pragma once

struct Vector3;
struct BaseTransform;
namespace SoLib {

	/// @brief 線形補間
	/// @tparam T 任意の型
	/// @param Start 始点
	/// @param End 終点
	/// @param t 比率
	/// @return 補間した値
	template<typename T>
	inline T Lerp(const T &Start, const T &End, float t) {
		return Start + (End - Start) * t;
	}

	template<>
	BaseTransform Lerp(const BaseTransform &start, const BaseTransform &end, float t);

	/// @brief 球面線形補間
	/// @param Start 始点のオイラー角
	/// @param End 終点のオイラー角
	/// @param t 比率
	/// @return 補間したオイラー角
	Vector3 Slerp(const Vector3 &Start, const Vector3 &End, float t);
	
}