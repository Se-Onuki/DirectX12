#pragma once
#include "Vector3.h"
#include "Matrix4x4.h"
#include "Math.hpp"

#include <d3d12.h>
#include <wrl.h>
#include "../../Engine/DirectBase/Base/CBuffer.h"

/// @brief GPUに渡す構造体
struct TransformMatrix {
	Matrix4x4 World;
	Matrix4x4 WorldInverseTranspose;
};

/// @brief Transformクラス
struct BaseTransform {
	BaseTransform &operator=(const BaseTransform &other) = default;

	using map_struct = TransformMatrix;
	CMapTarget<TransformMatrix> mapTarget_;


	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	Vector3 scale = { 1.f,1.f,1.f };
	Vector3 rotate{};
	Vector3 translate{};

	Matrix4x4 matWorld_{};	// ローカル . ワールド変換
	const BaseTransform *parent_ = nullptr;	// 親へのアドレス

public:

	const Vector3 &GetGrobalPos() const { return *reinterpret_cast<const Vector3 *const>(matWorld_.m[3]); }

	Matrix4x4 Affine() const;

	void CalcMatrix();
	void TransferMatrix();
	void UpdateMatrix();

	bool ImGuiWidget(const std::string &name = "");
	bool ImGuiWidget2D();

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