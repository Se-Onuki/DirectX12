#pragma once
#include "../../Header/Math/Vector3.h"
#include "../../Header/Math/Vector4.h"
#include <stdint.h>
#include <wrl.h>
#include <d3d12.h>

namespace Light {
	struct Direction {
		Vector4 color;		// 色(RGBA)
		Vector3 direction;	// ライトの向き
		float intensity;	// 輝度

		uint32_t lightingPattern[4] = { 2,2,2,2 };
	};
}

class DirectionLight {
	//
	//	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//
	//public:
	//	struct Direction {
	//		Vector4 color;		// 色(RGBA)
	//		Vector3 direction;	// ライトの向き
	//		float intensity;	// 輝度
	//		uint32_t lightingPattern;	// ライティングの種類
	//	};
	//
	//	enum class Pattern {
	//		kNone,
	//		kLambert,
	//		kHalfLambert
	//	};
	//
	//	void Init();
	//	void SetLight();
	//
	//private:
	//
	//	ComPtr<ID3D12Resource> lightResource_ = nullptr;
	//	Direction *lightData_ = nullptr;

};