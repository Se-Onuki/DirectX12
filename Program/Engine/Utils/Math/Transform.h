/// @file Transform.h
/// @brief 座標データのクラス
/// @author ONUKI seiya
#pragma once
#include "Math.hpp"
#include "Matrix4x4.h"
#include "Vector3.h"

#include "../../Engine/DirectBase/Base/CBuffer.h"
#include <d3d12.h>
#include <wrl.h>

/// @brief GPUに渡す構造体
struct TransformMatrix
{
	Matrix4x4 World;
	Matrix4x4 WorldInverseTranspose;
};

/// @brief Transformクラス
struct BaseTransform
{
	BaseTransform &operator=(const BaseTransform &other) = default;

	using map_struct = TransformMatrix;
	CMapTarget<TransformMatrix> mapTarget_;

	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	Vector3 scale = {1.f, 1.f, 1.f};
	Quaternion rotate = Quaternion::Identity;
	Vector3 translate{};

	Matrix4x4 matWorld_{};					// ローカル . ワールド変換
	const BaseTransform *parent_ = nullptr; // 親へのアドレス

public:
	/// @brief グローバル座標を取得
	const Vector3 &GetGrobalPos() const { return *reinterpret_cast<const Vector3 *const>(matWorld_.m[3].data()); }

	/// @brief アフィン行列を取得
	Matrix4x4 Affine() const;

	/// @brief 行列を計算
	void CalcMatrix();
	/// @brief 行列を転送
	void TransferMatrix();
	/// @brief 行列を計算して転送
	void UpdateMatrix();

	/// @brief ImGuiウィジェット
	bool ImGuiWidget(const std::string &name = "");
	bool ImGuiWidget2D();

	/// @brief 行列をSRTに変換
	void MatToSRT(const Matrix4x4 &mat);

	/// @brief 純粋な親子関係の構築
	/// @param parent 親のTransform
	void SetParent(const BaseTransform &parent);

	/// @brief グローバル座標を維持した親子関係の構築
	/// @param parent 親のTransform
	void ConnectParent(const BaseTransform &parent);

	/// @brief グローバル座標を維持した親子関係の破棄
	void DisConnectParent();

private:
};

/// @brief Transform構造体 (メモリ確保あり)
using Transform = ConstantContainer<BaseTransform>;

///
///	- using Transform = ConstantContainer<BaseTransform>;
///		メモリ確保を行うTransformの
///
/// - struct BaseTransform
///		メモリを確保しないTransform構造体。
///		StructuredBufferなどで使用する。
///

struct SimpleTransformQuaternion
{
	Vector3 scale_ = Vector3::one;
	Quaternion rotate_ = Quaternion::Identity;
	Vector3 translate_ = {};
	/// @brief アフィン行列を計算
	inline Matrix4x4 Affine() const { return SoLib::Affine(scale_, rotate_, translate_); }
	inline Matrix4x4 Affine(const Matrix4x4 &parent) const { return Affine() * parent; }
};

struct SimpleTransformEuler
{
	Vector3 scale_ = Vector3::one;
	SoLib::Euler rotate_;
	Vector3 translate_ = {};

	/// @brief アフィン行列を計算
	inline Matrix4x4 Affine() const { return SoLib::Affine(scale_, rotate_, translate_); }
	inline Matrix4x4 Affine(const Matrix4x4 &parent) const { return Affine() * parent; }
};

/// @brief GPUに転送するためのクラス
struct MatBase2D
{
	Vector2 scale_;
	Vector2 rotateSinCos_;
	Vector2 translate_;
};

/// @brief 2次元用姿勢クラス
struct Transform2D
{
	Vector2 scale_;
	Angle::Radian rotate_;
	Vector2 translate_;

	/// @brief データ転送用のクラスを構築する
	/// @return 転送用のデータ
	MatBase2D Generate() const
	{

		return MatBase2D{
			.scale_ = scale_,
			.rotateSinCos_ = {std::sin(rotate_.Get()), std::cos(rotate_.Get())},
			.translate_ = translate_};
	};
};

namespace SoLib {
	inline Transform2D Lerp(const Transform2D &str, const Transform2D &end, const float t)
	{
		Transform2D result;
		result.scale_ = SoLib::Lerp(str, end, &Transform2D::scale_, t);
		result.rotate_ = Angle::Radian{Angle::Lerp(str.rotate_.Get(), end.rotate_.Get(), t)};
		result.translate_ = SoLib::Lerp(str, end, &Transform2D::translate_, t);

		return result;
	}
}