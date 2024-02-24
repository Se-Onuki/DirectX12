#pragma once
#include "Matrix3x3.h"
#include "Vector3.h"
#include <array>
#include <tuple>

struct TransformMatrix {

#pragma warning(push)  // 現在の警告のステータスを保存する
#pragma warning(disable : 4201)  // C4201警告を無視する

	// 無名共用体
	union {
		struct {
			Matrix3x3 rotMat;
			Vector3 translate;
		};
		std::array<std::array<float, 3u>, 4u> m;
		std::array<Vector3, 4u> vec;
		std::array<float, 12u> arr;
	};

#pragma warning(pop)  // 以前の警告のステータスに戻す

	static const TransformMatrix one;

	const TransformMatrix &Identity() const { return one; }

	TransformMatrix Inverse() const;

	TransformMatrix InverseRT() const;

	/// @brief 回転行列を取得する
	/// @return 回転行列
	Matrix3x3 &GetRotateMat() { return rotMat; }
	const Matrix3x3 &GetRotateMat() const { return rotMat; }

	/// @brief 右を示すベクトルを取得する
	/// @return 右ベクトル
	Vector3 &GetRightAxis() { return vec[0]; }
	const Vector3 &GetRightAxis() const { return vec[0]; }

	/// @brief 上を示すベクトルを取得する
	/// @return 上ベクトル
	Vector3 &GetUpAxis() { return vec[1]; }
	const Vector3 &GetUpAxis() const { return vec[1]; }

	/// @brief 前を示すベクトルを取得する
	/// @return 前ベクトル
	Vector3 &GetFrontAxis() { return vec[2]; }
	const Vector3 &GetFrontAxis() const { return vec[2]; }

	/// @brief 平行移動要素を取得する
	/// @return 平行移動要素
	Vector3 &GetTranslate() { return translate; }
	const Vector3 &GetTranslate() const { return translate; }
};

TransformMatrix operator*(const TransformMatrix &left, const TransformMatrix &right);