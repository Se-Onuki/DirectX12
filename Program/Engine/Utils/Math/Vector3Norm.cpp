/// @file Vector3Norm.cpp
/// @brief NormalizeされたVector3
/// @author ONUKI seiya
#include "Vector3Norm.h"
namespace SoLib {

	Vector3Norm::Vector3Norm(const Vector3 &other) {
		*static_cast<Vector3 *>(this) = other.Normalize();
	}

	Vector3Norm::Vector3Norm(Vector3 &&other) {
		*static_cast<Vector3 *>(this) = other.Normalize();
	}

	Vector3Norm &Vector3Norm::operator=(const Vector3 &other) {
		*static_cast<Vector3 *>(this) = other.Normalize();
		return *this;
	}

	Vector3Norm &Vector3Norm::operator=(Vector3 &&other) {
		*static_cast<Vector3 *>(this) = other.Normalize();
		return *this;
	}

}