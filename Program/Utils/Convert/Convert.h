#pragma once
#include "assimp/vector3.h"
#include "assimp/quaternion.h"
#include "../Math/Transform.h"

namespace SoLib {
	// 右手座標系を左手座標系に変換する
	SimpleTransformQuaternion Convert(const aiVector3D &scale, const aiQuaternion &rotate, const aiVector3D &translate);
}