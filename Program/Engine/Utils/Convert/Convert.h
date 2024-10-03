#pragma once
#include "assimp/vector3.h"
#include "assimp/quaternion.h"
#include "../Math/Transform.h"
#include "../SoLib/SoLib_Traits.h"
#include <assimp/matrix4x4.h>
#include "assimp/vector3.h"
#include "assimp/types.h"
#include "assimp/color4.h"

namespace SoLib {
	template <IsRealType Result, IsRealType... Args>
	Result Convert(const Args&...args);

	// 右手座標系を左手座標系に変換する
	template<>
	SimpleTransformQuaternion Convert(const aiVector3D &scale, const aiQuaternion &rotate, const aiVector3D &translate);

	// 右手座標系を左手座標系に変換する
	template<>
	SimpleTransformQuaternion Convert(const aiMatrix4x4 &transform);

	/// @brief 行列表現をassimp標準形式からDirectX標準形式に変換する
	/// @param transform assimp行列
	/// @return DirectX行列
	template<>
	Matrix4x4 Convert(const aiMatrix4x4 &transform);

	template <IsRealType T>
	struct ConvertHelper;

	template <>
	struct ConvertHelper<Vector3> final {
		using FromAssimp = aiVector3D;
	};
	template <>
	struct ConvertHelper<Color::RGB3> final {
		using FromAssimp = aiColor3D;
	};
	template <>
	struct ConvertHelper<Color::RGB4> final {
		using FromAssimp = aiColor4D;
	};

	template <>
	struct ConvertHelper<aiVector3D> final {
		using ToSolEngine = aiVector3D;
	};
	template <>
	struct ConvertHelper<aiColor3D> final {
		using ToSolEngine = Color::RGB3;
	};
	template <>
	struct ConvertHelper<aiColor4D> final {
		using ToSolEngine = Color::RGB4;
	};

}