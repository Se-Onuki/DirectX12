#pragma once
#include <array>
#include "Matrix3x3.h"
#include "Vector3.h"

struct TransformMatrix {
	std::array<std::array<float, 3u>, 4u> m;

	/// @brief 回転行列を取得する
	/// @return 回転行列
	Matrix3x3 &GetRotateMat() { return *reinterpret_cast<Matrix3x3 *>(m.data()->data()); }
	const Matrix3x3 &GetRotateMat() const { return *reinterpret_cast<const Matrix3x3 *>(m.data()->data()); }

	Vector3 &GetTranslate() { return *reinterpret_cast<Vector3 *>(m[3].data()); }
	const Vector3 &GetTranslate() const { return *reinterpret_cast<const Vector3 *>(m[3].data()); }

};