#pragma once
#include "../../Header/Math/Matrix4x4.h"
#include "../../Header/Math/Vector3.h"
#include "../../Header/Math/Vector2.h"
#include <wrl.h>
#include <d3d12.h>
#include "../../Header/Math/Transform.h"

class Sprite {

	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// 透視投影行列
	static Matrix4x4 matProjection_;
	// [借用] デバイス
	static ID3D12Device *device_;

public:
	struct VertexData {
		Vector3 position;
		float pad0;
		Vector2 uv;
	};

	struct ConstData {
		Vector4 color;
		Matrix4x4 matWorldProjection;
	};

	Sprite() = default;
	~Sprite() = default;

	static void StaticInit(ID3D12Device *const device);
private:

	void CreateVertex();



private:




private:

	// マッピング無しTransform
	Transform transform_;
	// テクスチャID
	uint32_t textureHaundle_ = 1;

public:

	void SetTextureHaundle(const uint32_t textureHaundle) {
		textureHaundle_ = textureHaundle;
	}

	void Init();
	void Draw();

};