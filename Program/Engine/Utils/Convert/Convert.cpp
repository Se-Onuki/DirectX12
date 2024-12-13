/// @file Convert.cpp
/// @brief 変換を行う
/// @author ONUKI seiya
#include "Convert.h"

namespace  SoLib {
	template<>
	SimpleTransformQuaternion Convert(const aiVector3D &scale, const aiQuaternion &rotate, const aiVector3D &translate)
	{
		SimpleTransformQuaternion result;
		result.scale_ = { scale.x, scale.y, scale.z };
		result.rotate_ = { rotate.x, -rotate.y, -rotate.z, rotate.w };
		result.translate_ = { -translate.x, translate.y, translate.z };
		return result;
	}

	template<>
	SimpleTransformQuaternion Convert(const aiMatrix4x4 &transform)
	{
		aiVector3D scale, translate;
		aiQuaternion rotate;
		// Transformを取得
		transform.Decompose(scale, rotate, translate);
		return Convert<SimpleTransformQuaternion>(scale, rotate, translate);
	}

	template<>
	Matrix4x4 Convert(const aiMatrix4x4 &transform)
	{
		return Convert<SimpleTransformQuaternion>(transform).Affine();

	}
}