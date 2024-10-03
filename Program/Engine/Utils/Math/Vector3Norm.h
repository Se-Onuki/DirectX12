#pragma once
#include "Vector3.h"

struct Vector3Norm final : public Vector3 {

	Vector3Norm() = default;
	Vector3Norm(float x, float y, float z) : Vector3::Vector3(Vector3{ x,y,z }.Nomalize()) {};

	Vector3Norm(const Vector3Norm &) = default;
	Vector3Norm(Vector3Norm &&) = default;

	Vector3Norm &operator=(const Vector3Norm &) = default;
	Vector3Norm &operator=(Vector3Norm &&) = default;

	Vector3Norm(const Vector3 &other);
	Vector3Norm(Vector3 &&other);

	/// @brief Vector3からの代入
	/// @param other 代入元
	/// @return 正規化された三次元ベクトル
	Vector3Norm &operator=(const Vector3 &other);
	Vector3Norm &operator=(Vector3 &&other);

	/// @brief Vector3への変換
	inline operator Vector3 &() { return *static_cast<Vector3 *>(this); }
	/// @brief Vector3への変換
	inline operator const Vector3 &() const { return *static_cast<const Vector3 *>(this); }

	/// @brief 全部 0 であった場合はfalse
	inline operator bool() const { return x != 0.f || y != 0.f || z != 0.f; }

};