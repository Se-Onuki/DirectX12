#pragma once
#include "../../Header/Math/Matrix4x4.h"
#include "../../Header/Math/Vector3.h"
#include <wrl.h>
#include <d3d12.h>

class Sprite {

	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	static Matrix4x4 matProjection_;
	static ID3D12Device *device_;

public:
	Sprite() = default;
	~Sprite() = default;

	static void StaticInit(ID3D12Device *const device);
private:

	Vector3 scale_;
	Vector3 rotate_;
	Vector3 translate_;

	uint32_t textureHaundle_ = 1;

public:

	void Init();
	void Draw();

};