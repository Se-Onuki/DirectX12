#pragma once
#include "Matrix3x3.h"
#include "Vector3.h"
#include <array>

struct TransformMatrix {
    std::array<std::array<float, 3u>, 4u> m;

    TransformMatrix Inverse() const;
	
    TransformMatrix InverseRT() const;

    /// @brief 回転行列を取得する
    /// @return 回転行列
    Matrix3x3 &GetRotateMat() { return *reinterpret_cast<Matrix3x3 *>(m.data()->data()); }
    const Matrix3x3 &GetRotateMat() const { return *reinterpret_cast<const Matrix3x3 *>(m.data()->data()); }

    /// @brief 右を示すベクトルを取得する
    /// @return 右ベクトル
    Vector3 &GetRightAxis() { return *reinterpret_cast<Vector3 *>(m.data()->data()); }
    const Vector3 &GetRightAxis() const { return *reinterpret_cast<const Vector3 *>(m.data()->data()); }

    /// @brief 上を示すベクトルを取得する
    /// @return 上ベクトル
    Vector3 &GetUpAxis() { return *reinterpret_cast<Vector3 *>(m[1].data()); }
    const Vector3 &GetUpAxis() const { return *reinterpret_cast<const Vector3 *>(m[1].data()); }

    /// @brief 前を示すベクトルを取得する
    /// @return 前ベクトル
    Vector3 &GetFrontAxis() { return *reinterpret_cast<Vector3 *>(m[2].data()); }
    const Vector3 &GetFrontAxis() const { return *reinterpret_cast<const Vector3 *>(m[2].data()); }

    /// @brief 平行移動要素を取得する
    /// @return 平行移動要素
    Vector3 &GetTranslate() { return *reinterpret_cast<Vector3 *>(m[3].data()); }
    const Vector3 &GetTranslate() const { return *reinterpret_cast<const Vector3 *>(m[3].data()); }
};