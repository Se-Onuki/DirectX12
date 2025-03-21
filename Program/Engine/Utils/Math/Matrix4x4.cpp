/// @file Matrix4x4.cpp
/// @brief 4x4行列
/// @author ONUKI seiya
#include "Matrix4x4.h"

#include "Matrix2x2.h"
#include "Vector3.h"
#include "Vector3Norm.h"

#include <immintrin.h>

// #include <Novice.h>

#include "Math.hpp"
#include <cmath>

// void Matrix4x4::Printf(const int32_t  x, const int32_t  y) const {
//	for (int i = 0; i < 4; i++) {
//		for (int j = 0; j < 4; j++) {
//			/*Novice::ScreenPrintf(
//			    x + j * static_cast<int>(TextSize.x) * 2, y + i * static_cast<int>(TextSize.y),
//			    "%10.6f", m[i][j]);*/
//		}
//	}
// }
namespace SoLib {

	Matrix4x4 Matrix4x4::Inverse() const
	{
		Matrix4x4 calc = *this;
		Matrix4x4 out = Identity();

		for (size_t i = 0; i < 4; i++) {
			float value = calc.m[i][i];
			for (size_t k = 0; k < 4; k++) {
				calc.m[i][k] /= value;
				out.m[i][k] /= value;
			}
			for (size_t k = 0; k < 4; k++) {
				if (k != i) {
					value = -calc.m[k][i];
					for (size_t l = 0; l < 4; l++) {
						calc.m[k][l] += calc.m[i][l] * value;
						out.m[k][l] += out.m[i][l] * value;
					}
				}
			}
		}
		return out;
	}

	Matrix4x4 Matrix4x4::InverseRT() const
	{

		const __m128 mm3 = _mm_loadu_ps(m[3].data());
		return Matrix4x4{
			{m[0][0], m[1][0], m[2][0], 0.f},
			{m[0][1], m[1][1], m[2][1], 0.f},
			{m[0][2], m[1][2], m[2][2], 0.f},
			{-_mm_cvtss_f32(_mm_dp_ps(mm3, _mm_load_ps(m[0].data()), 0x71)),
			 -_mm_cvtss_f32(_mm_dp_ps(mm3, _mm_load_ps(m[1].data()), 0x71)),
			 -_mm_cvtss_f32(_mm_dp_ps(mm3, _mm_load_ps(m[2].data()), 0x71)), 1.f} };
	};

	Matrix4x4 Matrix4x4::Transpose() const
	{
		return Matrix4x4{
			this->m[0][0],
			this->m[1][0],
			this->m[2][0],
			this->m[3][0],
			this->m[0][1],
			this->m[1][1],
			this->m[2][1],
			this->m[3][1],
			this->m[0][2],
			this->m[1][2],
			this->m[2][2],
			this->m[3][2],
			this->m[0][3],
			this->m[1][3],
			this->m[2][3],
			this->m[3][3],
		};
	}

	Matrix4x4 Matrix4x4::Affine(const Vector3 &scale, const Vector3 &rotate, const Vector3 &translate)
	{
		enum SinCos {
			kSin,
			kCos,
		};

		const std::array<float, 2u> rotX{ std::sin(rotate.x), std::cos(rotate.x) };
		const std::array<float, 2u> rotY{ std::sin(rotate.y), std::cos(rotate.y) };
		const std::array<float, 2u> rotZ{ std::sin(rotate.z), std::cos(rotate.z) };

		return Matrix4x4{
			scale.x * (rotY[kCos] * rotZ[kCos]),
			scale.x * (rotY[kCos] * rotZ[kSin]),
			scale.x * (-rotY[kSin]),
			0,

			scale.y * (rotX[kSin] * rotY[kSin] * rotZ[kCos] -
					   rotX[kCos] * rotZ[kSin]),
			scale.y * (rotX[kSin] * rotY[kSin] * rotZ[kSin] +
					   rotX[kCos] * rotZ[kCos]),
			scale.y * (rotX[kSin] * rotY[kCos]),
			0,

			scale.z * (rotX[kCos] * rotY[kSin] * rotZ[kCos] +
					   rotX[kSin] * rotZ[kSin]),
			scale.z * (rotX[kCos] * rotY[kSin] * rotZ[kSin] -
					   rotX[kSin] * rotZ[kCos]),
			scale.z * (rotX[kCos] * rotY[kCos]),
			0,

			translate.x,
			translate.y,
			translate.z,
			1 };
	}

	Matrix4x4 Matrix4x4::EulerRotate(EulerAngle eulerAngle, float angle)
	{
		switch (eulerAngle) {
		case Matrix4x4::Pitch:
			return Matrix4x4{
				Vector4{
					1,
					0,
					0,
					0,
				},
				Vector4{
					0,
					std::cos(angle),
					std::sin(angle),
					0,
				},
				Vector4{
					0,
					-std::sin(angle),
					std::cos(angle),
					0,
				},
				Vector4{
					0,
					0,
					0,
					1} };
			break;
		case Matrix4x4::Yaw:
			return Matrix4x4{ std::cos(angle), 0, -std::sin(angle), 0, 0, 1, 0, 0,
							 std::sin(angle), 0, std::cos(angle), 0, 0, 0, 0, 1 };
			break;
		case Matrix4x4::Roll:
			return Matrix4x4{
				std::cos(angle),
				std::sin(angle),
				0,
				0,
				-std::sin(angle),
				std::cos(angle),
				0,
				0,
				0,
				0,
				1,
				0,
				0,
				0,
				0,
				1 };
			break;
		default:
			return Identity();
			break;
		}
	}

	Matrix4x4 Matrix4x4::EulerRotate(const Vector3 &rotate)
	{
		return Matrix4x4{
			std::cos(rotate.y) * std::cos(rotate.z),
			std::cos(rotate.y) * std::sin(rotate.z),
			-std::sin(rotate.y),
			0,

			std::sin(rotate.x) * std::sin(rotate.y) * std::cos(rotate.z) -
				std::cos(rotate.x) * std::sin(rotate.z),
			std::sin(rotate.x) * std::sin(rotate.y) * std::sin(rotate.z) +
				std::cos(rotate.x) * std::cos(rotate.z),
			std::sin(rotate.x) * std::cos(rotate.y),
			0,

			std::cos(rotate.x) * std::sin(rotate.y) * std::cos(rotate.z) +
				std::sin(rotate.x) * std::sin(rotate.z),
			std::cos(rotate.x) * std::sin(rotate.y) * std::sin(rotate.z) -
				std::sin(rotate.x) * std::cos(rotate.z),
			std::cos(rotate.x) * std::cos(rotate.y),
			0,

			0,
			0,
			0,
			1 };
	}

	Matrix4x4 Matrix4x4::AnyAngleRotate(const Vector3Norm &axis, const float angle)
	{

		const float cosTheta = std::cos(angle);
		const float sinTheta = std::sin(angle);

		return AnyAngleRotate(axis, cosTheta, sinTheta);
	}

	Matrix4x4 Matrix4x4::AnyAngleRotate(const Vector3Norm &axis, const float cos, const float sin)
	{

		const float minusCosTheta = 1.f - cos;

		return Matrix4x4{
			Vector4{std::powf(axis.x, 2) * minusCosTheta + cos, axis.x * axis.y * minusCosTheta + axis.z * sin, axis.x * axis.z * minusCosTheta - axis.y * sin, 0.f},
			Vector4{axis.x * axis.y * minusCosTheta - axis.z * sin, std::powf(axis.y, 2) * minusCosTheta + cos, axis.y * axis.z * minusCosTheta + axis.x * sin, 0.f},
			Vector4{axis.x * axis.z * minusCosTheta + axis.y * sin, axis.y * axis.z * minusCosTheta - axis.x * sin, std::powf(axis.z, 2) * minusCosTheta + cos, 0.f},
			Vector4{0.f, 0.f, 0.f, 1.f},
		};
	}

	Matrix4x4 Matrix4x4::DirectionToDirection(const Vector3Norm &from, const Vector3Norm &to)
	{

		const Vector3 &u = from;
		const Vector3 &v = to;

		const float dot = u * v;
		const Vector3 cross = u.cross(v);
		Vector3Norm axis = cross;

		if (dot == -1.f) {
			if (u.x != 0.f || u.y != 0.f) {
				axis = Vector3{ u.y, -u.x, 0.f };
			}
			else if (u.x != 0.f || u.z != 0.f) {
				axis = Vector3{ u.z, 0.f, -u.x };
			}
		}

		return AnyAngleRotate(axis, dot, cross.Length());
	}

	Matrix4x4 Matrix4x4::operator+(const Matrix4x4 &Second) const
	{
		return (Matrix4x4{
			this->m[0][0] + Second.m[0][0], this->m[0][1] + Second.m[0][1],
			this->m[0][2] + Second.m[0][2], this->m[0][3] + Second.m[0][3],

			this->m[1][0] + Second.m[1][0], this->m[1][1] + Second.m[1][1],
			this->m[1][2] + Second.m[1][2], this->m[1][3] + Second.m[1][3],

			this->m[2][0] + Second.m[2][0], this->m[2][1] + Second.m[2][1],
			this->m[2][2] + Second.m[2][2], this->m[2][3] + Second.m[2][3],

			this->m[3][0] + Second.m[3][0], this->m[3][1] + Second.m[3][1],
			this->m[3][2] + Second.m[3][2], this->m[3][3] + Second.m[3][3] });
	}

	Matrix4x4 Matrix4x4::operator-(const Matrix4x4 &Second) const
	{
		return (Matrix4x4{
			this->m[0][0] - Second.m[0][0], this->m[0][1] - Second.m[0][1],
			this->m[0][2] - Second.m[0][2], this->m[0][3] - Second.m[0][3],

			this->m[1][0] - Second.m[1][0], this->m[1][1] - Second.m[1][1],
			this->m[1][2] - Second.m[1][2], this->m[1][3] - Second.m[1][3],

			this->m[2][0] - Second.m[2][0], this->m[2][1] - Second.m[2][1],
			this->m[2][2] - Second.m[2][2], this->m[2][3] - Second.m[2][3],

			this->m[3][0] - Second.m[3][0], this->m[3][1] - Second.m[3][1],
			this->m[3][2] - Second.m[3][2], this->m[3][3] - Second.m[3][3] });
	}

	Matrix4x4 Matrix4x4::operator*(const float &Second) const
	{
		return (Matrix4x4{
			this->m[0][0] * Second, this->m[0][1] * Second, this->m[0][2] * Second,
			this->m[0][3] * Second,

			this->m[1][0] * Second, this->m[1][1] * Second, this->m[1][2] * Second,
			this->m[1][3] * Second,

			this->m[2][0] * Second, this->m[2][1] * Second, this->m[2][2] * Second,
			this->m[2][3] * Second,

			this->m[3][0] * Second, this->m[3][1] * Second, this->m[3][2] * Second,
			this->m[3][3] * Second });
	}

	Matrix4x4 Matrix4x4::operator/(const float &Second) const
	{
		return (Matrix4x4{
			this->m[0][0] / Second, this->m[0][1] / Second, this->m[0][2] / Second,
			this->m[0][3] / Second,

			this->m[1][0] / Second, this->m[1][1] / Second, this->m[1][2] / Second,
			this->m[1][3] / Second,

			this->m[2][0] / Second, this->m[2][1] / Second, this->m[2][2] / Second,
			this->m[2][3] / Second,

			this->m[3][0] / Second, this->m[3][1] / Second, this->m[3][2] / Second,
			this->m[3][3] / Second });
	}

	Matrix4x4 Matrix4x4::operator+=(const Matrix4x4 &Second)
	{

		this->m[0][0] += Second.m[0][0], this->m[0][1] += Second.m[0][1];
		this->m[0][2] += Second.m[0][2], this->m[0][3] += Second.m[0][3];

		this->m[1][0] += Second.m[1][0], this->m[1][1] += Second.m[1][1];
		this->m[1][2] += Second.m[1][2], this->m[1][3] += Second.m[1][3];

		this->m[2][0] += Second.m[2][0], this->m[2][1] += Second.m[2][1];
		this->m[2][2] += Second.m[2][2], this->m[2][3] += Second.m[2][3];

		this->m[3][0] += Second.m[3][0], this->m[3][1] += Second.m[3][1];
		this->m[3][2] += Second.m[3][2], this->m[3][3] += Second.m[3][3];

		return *this;
	}

	Matrix4x4 Matrix4x4::operator-=(const Matrix4x4 &Second)
	{

		this->m[0][0] -= Second.m[0][0], this->m[0][1] -= Second.m[0][1];
		this->m[0][2] -= Second.m[0][2], this->m[0][3] -= Second.m[0][3];

		this->m[1][0] -= Second.m[1][0], this->m[1][1] -= Second.m[1][1];
		this->m[1][2] -= Second.m[1][2], this->m[1][3] -= Second.m[1][3];

		this->m[2][0] -= Second.m[2][0], this->m[2][1] -= Second.m[2][1];
		this->m[2][2] -= Second.m[2][2], this->m[2][3] -= Second.m[2][3];

		this->m[3][0] -= Second.m[3][0], this->m[3][1] -= Second.m[3][1];
		this->m[3][2] -= Second.m[3][2], this->m[3][3] -= Second.m[3][3];

		return *this;
	}

	Matrix4x4 Matrix4x4::operator*=(const float &Second)
	{

		this->m[0][0] *= Second;
		this->m[0][1] *= Second;
		this->m[0][2] *= Second;
		this->m[0][3] *= Second;

		this->m[1][0] *= Second;
		this->m[1][1] *= Second;
		this->m[1][2] *= Second;
		this->m[1][3] *= Second;

		this->m[2][0] *= Second;
		this->m[2][1] *= Second;
		this->m[2][2] *= Second;
		this->m[2][3] *= Second;

		this->m[3][0] *= Second;
		this->m[3][1] *= Second;
		this->m[3][2] *= Second;
		this->m[3][3] *= Second;

		return *this;
	}

	Matrix4x4 Matrix4x4::operator/=(const float &Second)
	{

		this->m[0][0] /= Second;
		this->m[0][1] /= Second;
		this->m[0][2] /= Second;
		this->m[0][3] /= Second;

		this->m[1][0] /= Second;
		this->m[1][1] /= Second;
		this->m[1][2] /= Second;
		this->m[1][3] /= Second;

		this->m[2][0] /= Second;
		this->m[2][1] /= Second;
		this->m[2][2] /= Second;
		this->m[2][3] /= Second;

		this->m[3][0] /= Second;
		this->m[3][1] /= Second;
		this->m[3][2] /= Second;
		this->m[3][3] /= Second;
		return *this;
	}

	bool operator==(const Matrix4x4 &left, const Matrix4x4 &right)
	{

		for (uint32_t y = 0; y < 4; y++) {
			for (uint32_t x = 0; x < 4; x++) {

				// どれか一つでも違ったらfalseを返す
				if (left.m[y][x] != right.m[y][x]) {
					return false;
				}
			}
		}

		return true;
	}

	bool operator!=(const Matrix4x4 &left, const Matrix4x4 &right)
	{

		return not(left == right);
	}

	Matrix4x4 Matrix4x4::LookAtLH(const Vector3 &cameraPosition, const Vector3 &cameraTarget, const Vector3 &cameraUpVector)
	{
		Vector3 zaxis = (cameraTarget - cameraPosition).Normalize();
		Vector3 xaxis = cameraUpVector.cross(zaxis).Normalize();
		Vector3 yaxis = zaxis.cross(xaxis);

		return Matrix4x4{
			{xaxis.x, yaxis.x, zaxis.x, 0.f},
			{xaxis.y, yaxis.y, zaxis.y, 0.f},
			{xaxis.z, yaxis.z, zaxis.z, 0.f},
			{xaxis * cameraPosition,
			 yaxis * cameraPosition,
			 zaxis * cameraPosition,
			 1.f},
		};
	}
}