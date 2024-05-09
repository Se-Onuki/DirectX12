#include "Convert.h"

SimpleTransformQuaternion SoLib::Convert(const aiVector3D &scale, const aiQuaternion &rotate, const aiVector3D &translate)
{
	SimpleTransformQuaternion result;
	result.scale_ = { scale.x, scale.y, scale.z };
	result.rotate_ = { rotate.x, -rotate.y, -rotate.z, rotate.w };
	result.translate_ = { -translate.x, translate.y, translate.z };
	return result;
}
