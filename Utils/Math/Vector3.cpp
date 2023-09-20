#include "Vector3.h"
#include "Math.hpp"
#include "Matrix4x4.h"
//#include <Novice.h>
#include <cassert>

//void Vector3::Printf(int x_, int y_) const {
//	/*Novice::ScreenPrintf(x_, y_, "%12.2f // %6.2f", this->x, Length());
//	Novice::ScreenPrintf(x_, y_ + static_cast<int>(TextSize.y), "%12.2f", this->y);
//	Novice::ScreenPrintf(x_, y_ + static_cast<int>(TextSize.y * 2), "%12.2f", this->z);*/
//}

Vector3 Vector3::operator*(const Matrix4x4& Second) const {
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

Vector3& Vector3::operator*=(const Matrix4x4& Second) {
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
