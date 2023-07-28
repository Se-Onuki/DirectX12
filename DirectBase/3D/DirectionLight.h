#pragma once
#include "../../Header/Math/Vector3.h"
#include "../../Header/Math/Vector4.h"

namespace Light {
	struct Direction {
		Vector4 color;		// 色(RGBA)
		Vector3 direction;	// ライトの向き
		float intensity;	// 輝度
	};
}