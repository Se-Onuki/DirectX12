#pragma once
#include "Vector3.h"
#include "Matrix4x4.h"

#include <d3d12.h>
#include <wrl.h>

struct Transform
{
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	struct TransformMatrix {
		Matrix4x4 World;
	};


	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;

	Matrix4x4 matWorld_;	// ローカル . ワールド変換
	Transform *parent_ = nullptr;	// 親へのアドレス

	ComPtr<ID3D12Resource> constBuffer_ = nullptr;
	TransformMatrix *mapData_ = nullptr;	// マップ済みデータ


	_NODISCARD Matrix4x4 Affine() const;

	void Init();

	void CreateConstBuffer();

	void SetMap();

	void UpdateMatrix();

	bool ImGuiWidget();
	bool ImGuiWidget2D();

	void Create(const Matrix4x4 &mat);
};