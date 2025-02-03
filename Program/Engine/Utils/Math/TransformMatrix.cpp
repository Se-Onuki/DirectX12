/// @file TransformMatrix.cpp
/// @brief Transform用のMatrix
/// @author ONUKI seiya
#include "TransformMatrix.h"
#include "Math.hpp"
#include "Matrix4x4.h"
namespace SoLib {

	const TransformMatrix TransformMatrix::one = TransformMatrix{ Matrix3x3::Identity() };

	TransformMatrix TransformMatrix::Inverse() const
	{
		TransformMatrix result;
		// サイズを一定にした行列
		Matrix3x3 tmp;
		std::transform(GetRotateMat().vec.cbegin(), GetRotateMat().vec.cend(), tmp.vec.begin(), [](const Vector3 &line) { return line.Nomalize(); });

		result.GetRotateMat() = tmp.Transpose();

		for (uint8_t i = 0u; i < 3u; i++) {
			// 転置前の行列とのドット積を代入 == 転置後の行列との行列乗算と同義
			result.GetTranslate().arr()[i] = -(GetTranslate() * tmp.vec[i]);
		}

		return result;
	}

	TransformMatrix TransformMatrix::InverseRT() const
	{
		TransformMatrix result;
		// 転置行列を代入
		result.GetRotateMat() = GetRotateMat().Transpose();

		for (uint8_t i = 0u; i < 3u; i++) {
			// 転置前の行列とのドット積を代入 == 転置後の行列との行列乗算と同義
			result.GetTranslate().arr()[i] = -(GetTranslate() * GetRotateMat().vec[i]);
		}
		return result;
	}

	TransformMatrix TransformMatrix::MakeMatrix(const Matrix4x4 &matrix)
	{
		TransformMatrix result;
		std::transform(matrix.vecs.cbegin(), matrix.vecs.end(), result.vec.begin(), [](const Vector4 &v) { return v.ToVec3(); });

		return result;
	}

	TransformMatrix TransformMatrix::MakeRotate(const SoLib::Math::Euler &rotate)
	{
		enum SinCos {
			kSin,
			kCos,
		};

		const std::array<float, 2u> rotX{ std::sin(rotate.x),std::cos(rotate.x) };
		const std::array<float, 2u> rotY{ std::sin(rotate.y),std::cos(rotate.y) };
		const std::array<float, 2u> rotZ{ std::sin(rotate.z),std::cos(rotate.z) };

		return TransformMatrix{
			.vec{
			Vector3{
				(rotY[kCos] * rotZ[kCos]),
				(rotY[kCos] * rotZ[kSin]),
				(-rotY[kSin])
			},

			Vector3{
				(rotX[kSin] * rotY[kSin] * rotZ[kCos] - rotX[kCos] * rotZ[kSin]),
				(rotX[kSin] * rotY[kSin] * rotZ[kSin] + rotX[kCos] * rotZ[kCos]),
				(rotX[kSin] * rotY[kCos])
			},
			Vector3{
				(rotX[kCos] * rotY[kSin] * rotZ[kCos] + rotX[kSin] * rotZ[kSin]),
				(rotX[kCos] * rotY[kSin] * rotZ[kSin] - rotX[kSin] * rotZ[kCos]),
				(rotX[kCos] * rotY[kCos])
			},
			Vector3{},
			}
		};
	}

	TransformMatrix TransformMatrix::Affine(const Vector3 &scale, const SoLib::Math::Euler &rotate, const Vector3 &translate) {
		enum SinCos {
			kSin,
			kCos,
		};

		const std::array<float, 2u> rotX{ std::sin(rotate.x),std::cos(rotate.x) };
		const std::array<float, 2u> rotY{ std::sin(rotate.y),std::cos(rotate.y) };
		const std::array<float, 2u> rotZ{ std::sin(rotate.z),std::cos(rotate.z) };

		return TransformMatrix{
			.vec{
			Vector3{
				scale.x * (rotY[kCos] * rotZ[kCos]),
				scale.x * (rotY[kCos] * rotZ[kSin]),
				scale.x * (-rotY[kSin])
			},

			Vector3{
				scale.y * (rotX[kSin] * rotY[kSin] * rotZ[kCos] -
							rotX[kCos] * rotZ[kSin]),
				scale.y * (rotX[kSin] * rotY[kSin] * rotZ[kSin] +
							rotX[kCos] * rotZ[kCos]),
				scale.y * (rotX[kSin] * rotY[kCos])
			},
			Vector3{
				scale.z * (rotX[kCos] * rotY[kSin] * rotZ[kCos] +
							rotX[kSin] * rotZ[kSin]),
				scale.z * (rotX[kCos] * rotY[kSin] * rotZ[kSin] -
							rotX[kSin] * rotZ[kCos]),
				scale.z * (rotX[kCos] * rotY[kCos])
			},
			Vector3{
				translate.x, translate.y, translate.z
			},
			} };
	}

	TransformMatrix TransformMatrix::AnyAngleRotate(const Vector3Norm &axis, const float angle) {

		const float cosTheta = std::cos(angle);
		const float sinTheta = std::sin(angle);

		return AnyAngleRotate(axis, cosTheta, sinTheta);
	}

	TransformMatrix TransformMatrix::AnyAngleRotate(const Vector3Norm &axis, const float cos, const float sin) {

		const float minusCosTheta = 1.f - cos;

		return TransformMatrix{
			.vec{
				Vector3{ std::powf(axis.x, 2) * minusCosTheta + cos,	axis.x * axis.y * minusCosTheta + axis.z * sin,		axis.x * axis.z * minusCosTheta - axis.y * sin },
				Vector3{ axis.x * axis.y * minusCosTheta - axis.z * sin,	std::powf(axis.y, 2) * minusCosTheta + cos,	axis.y * axis.z * minusCosTheta + axis.x * sin },
				Vector3{ axis.x * axis.z * minusCosTheta + axis.y * sin,	axis.y * axis.z * minusCosTheta - axis.x * sin,		std::powf(axis.z,2) * minusCosTheta + cos },
				Vector3{  },
			}
		};
	}

	TransformMatrix TransformMatrix::DirectionToDirection(const Vector3Norm &from, const Vector3Norm &to) {

		const Vector3 &u = from;
		const Vector3 &v = to;

		const float dot = u * v;
		const Vector3 cross = u.cross(v);
		Vector3Norm axis = cross;

		if (dot == -1.f) {
			if (u.x != 0.f || u.y != 0.f) {
				axis = Vector3{ u.y,-u.x,0.f };
			}
			else if (u.x != 0.f || u.z != 0.f) {
				axis = Vector3{ u.z,0.f,-u.x };
			}
		}

		return AnyAngleRotate(axis, dot, cross.Length());
	}
}