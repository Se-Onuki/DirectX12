/// @file Vector3.cpp
/// @brief Vector3
/// @author ONUKI seiya
#include "Vector3.h"
#include "Math.hpp"
#include "Matrix4x4.h"
//#include <Novice.h>
#include <cassert>
namespace SoLib {

	const Vector3 Vector3::zero = Vector3{ 0.f,0.f,0.f };

	const Vector3 Vector3::right = Vector3{ 1.f,0.f,0.f };
	const Vector3 Vector3::up = Vector3{ 0.f,1.f,0.f };
	const Vector3 Vector3::front = Vector3{ 0.f,0.f,1.f };

	const Vector3 Vector3::one = Vector3{ 1.f,1.f,1.f };

	//void Vector3::Printf(int x_, int y_) const {
	//	/*Novice::ScreenPrintf(x_, y_, "%12.2f // %6.2f", this->x, Length());
	//	Novice::ScreenPrintf(x_, y_ + static_cast<int>(TextSize.y), "%12.2f", this->y);
	//	Novice::ScreenPrintf(x_, y_ + static_cast<int>(TextSize.y * 2), "%12.2f", this->z);*/
	//}

	Vector3 Vector3::operator*(const Matrix4x4 &Second) const {
		Vector3 result;

		result.x = this->x * Second.m[0][0] + this->y * Second.m[1][0] + this->z * Second.m[2][0] +
			1.0f * Second.m[3][0];
		result.y = this->x * Second.m[0][1] + this->y * Second.m[1][1] + this->z * Second.m[2][1] +
			1.0f * Second.m[3][1];
		result.z = this->x * Second.m[0][2] + this->y * Second.m[1][2] + this->z * Second.m[2][2] +
			1.0f * Second.m[3][2];
		const float w = this->x * Second.m[0][3] + this->y * Second.m[1][3] + this->z * Second.m[2][3] +
			1.0f * Second.m[3][3];
		assert(w != 0.0f);
		return result / w; // 演算子のオーバーライド
	}

	Vector3 &Vector3::operator*=(const Matrix4x4 &Second) {
		Vector3 result;

		result.x = this->x * Second.m[0][0] + this->y * Second.m[1][0] + this->z * Second.m[2][0] +
			1.0f * Second.m[3][0];
		result.y = this->x * Second.m[0][1] + this->y * Second.m[1][1] + this->z * Second.m[2][1] +
			1.0f * Second.m[3][1];
		result.z = this->x * Second.m[0][2] + this->y * Second.m[1][2] + this->z * Second.m[2][2] +
			1.0f * Second.m[3][2];
		const float w = this->x * Second.m[0][3] + this->y * Second.m[1][3] + this->z * Second.m[2][3] +
			1.0f * Second.m[3][3];
		assert(w != 0.0f);
		return *this = (result / w); // 演算子のオーバーライド
	}

	Vector3 Vector3::Scaling(const Vector3 &other) const {
		Vector3 result{};

		for (uint32_t i = 0u; i < 3u; ++i) {
			if (other.data()[i] != 0.f) {
				result.data()[i] = this->data()[i] / other.data()[i];
			}
		}

		return result;
	}
}