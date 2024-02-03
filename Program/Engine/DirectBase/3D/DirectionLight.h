#pragma once
#include "../../Utils/Math/Euler.h"
#include "../../Utils/Math/Vector3.h"
#include "../../Utils/Math/Vector4.h"
#include <stdint.h>
#include <wrl.h>
#include <d3d12.h>
#include <memory>

namespace Light {
	struct Direction {
		Vector4 color;		// 色(RGBA)
		Vector3 direction;	// ライトの向き
		float intensity;	// 輝度

		int32_t pattern;
	};
}

class DirectionLight {

	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:

	DirectionLight() = default;
	~DirectionLight() = default;

	struct Direction {
		Vector4 color;		// 色(RGBA)
		Vector3 direction;	// ライトの向き
		float intensity;	// 輝度
		int32_t pattern;	// ライティングの種類
	};

	enum class Pattern {
		kNone,
		kLambert,
		kHalfLambert
	};

	void Init();
	void SetLight(ID3D12GraphicsCommandList *const commandList);

	void ImGuiWidget();

	[[nodiscard]] static std::unique_ptr<DirectionLight> Create();

private:

	ComPtr<ID3D12Resource> lightResource_ = nullptr;
	Direction *lightData_ = nullptr;
	SoLib::Math::Euler euler_;

};