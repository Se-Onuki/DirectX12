#pragma once
#include "SoLib_Traits.h"
#include "../../externals/nlohmann/json.hpp"

#include "../Math/Matrix2x2.h"
#include "../Math/Matrix3x3.h"
#include "../Math/Matrix4x4.h"
#include "../Math/Quaternion.h"
#include "../Math/Vector2.h"
#include "../Math/Vector3.h"
#include "../Math/Vector4.h"

namespace SoLib {

	template<SoLib::IsNotPointer T>
	T cast(const nlohmann::json &json) { return T{}; }

	template<>
	Vector2 cast(const nlohmann::json &json);

	template<>
	Vector3 cast(const nlohmann::json &json);

	template<>
	Vector4 cast(const nlohmann::json &json);

}
