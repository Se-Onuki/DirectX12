#pragma once
#include "Vector3.h"
#include "Matrix4x4.h"

#include <d3d12.h>
#include <wrl.h>
#include "../../Engine/DirectBase/Base/CBuffer.h"

struct Transform;

struct SRT {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;

	SRT &operator=(const Transform &other);
	static SRT MatToSRT(const Matrix4x4 &mat);
};

struct Transform {
	Transform &operator=(const Transform &other) = default;

	Transform &operator=(const SRT &other);
	//Transform &operator=(Transform &&other) = default;

	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	struct TransformMatrix {
		Matrix4x4 World;
	};


	Vector3 scale = { 1.f,1.f,1.f };
	Vector3 rotate{};
	Vector3 translate{};

	Matrix4x4 matWorld_{};	// ローカル . ワールド変換
	const Transform *parent_ = nullptr;	// 親へのアドレス

	//ComPtr<ID3D12Resource> constBuffer_ = nullptr;
	//TransformMatrix *mapData_ = nullptr;	// マップ済みデータ

	CBuffer<TransformMatrix> mapBuffer_;

	Matrix4x4 Affine() const;

	Vector3 GetWorldPosition() const { return *reinterpret_cast<const Vector3 *>(&matWorld_.m[3]); }

	void InitResource();

	void CalcMatrix();
	void TransferMatrix();
	void UpdateMatrix();

	bool ImGuiWidget();
	bool ImGuiWidget2D();

	/// @brief 行列からSRTを算出する
	/// @param mat SRT行列
	void MatToSRT(const Matrix4x4 &mat);

	/// @brief 純粋な親子関係の構築
	/// @param parent 親のTransform
	void SetParent(const Transform &parent);

	/// @brief グローバル座標を維持した親子関係の構築
	/// @param parent 親のTransform
	void ConnectParent(const Transform &parent);

	/// @brief グローバル座標を維持した親子関係の破棄
	void DisConnectParent();
private:
};