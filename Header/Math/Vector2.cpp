#include "Vector2.h"

//#include <Novice.h>
#define _USE_MATH_DEFINES
#include <math.h>


#include"Math.hpp"

#include <assert.h>


void Vector2::Reset() {
	*this = ZeroVector2;
}

Vector2 Vector2::Rotate(const float &theta) const {
	return *this * MakeRotateMatrix(theta);
}

float Vector2::Length() const {
	return sqrtf(powf(x, 2) + powf(y, 2));
}

Vector2 Vector2::Nomalize() const {
	float Length = this->Length();
	if (Length != 0) {
		return *this / Length;
	}
	else {
		return ZeroVector2;
	}
}
//
//void Vector2::Printf(int x_, int y_) const {
////	Novice::ScreenPrintf(x_, y_, "%12.2f // %6.2f", this->x, Length());
////	Novice::ScreenPrintf(x_, y_ + TextHeight, "%12.2f", this->y);
//}

Vector2 Vector2::operator+(const Vector2 &Second) const {
	return { (this->x) + (Second.x), (this->y) + (Second.y) };
}

Vector2 Vector2::operator-(const Vector2 &Second) const {
	return { (this->x) - (Second.x), (this->y) - (Second.y) };
}

void Vector2::operator +=(const Vector2 &Second) {
	this->x += Second.x;
	this->y += Second.y;
}

void Vector2::operator -=(const Vector2 &Second) {
	this->x -= Second.x;
	this->y -= Second.y;
}

Vector2 Vector2::operator*(const float &Second) const {
	return { (this->x) * (Second), (this->y) * (Second) };
}

Vector2 Vector2::operator/(const float &Second) const {
	return { (this->x) / (Second), (this->y) / (Second) };
}

void Vector2::operator *=(const float &Second) {
	this->x *= Second;
	this->y *= Second;
}


void Vector2::operator /=(const float &Second) {
	this->x /= Second;
	this->y /= Second;
}

Vector2 Vector2::operator*(const Matrix2x2 &Second) const {
	const Vector2 b[2]{ {Second.m[0][0],Second.m[1][0]},{Second.m[0][1],Second.m[1][1]} };
	return Vector2{
		(*this* b[0]),
		(*this* b[1])
	};
}

void Vector2::operator*=(const Matrix2x2 &Second) {
	const Vector2 b[2]{ {Second.m[0][0],Second.m[1][0]},{Second.m[0][1],Second.m[1][1]} };
	*this = {
		(*this* b[0]),
		(*this* b[1])
	};
}

Vector2 Vector2::operator*(const Matrix3x3 &matrix) const {
	Vector2 result;

	result.x = x * matrix.m[0][0] + y * matrix.m[1][0] + 1.0f * matrix.m[2][0];
	result.y = x * matrix.m[0][1] + y * matrix.m[1][1] + 1.0f * matrix.m[2][1];
	const float w = x * matrix.m[0][2] + y * matrix.m[1][2] + 1.0f * matrix.m[2][2];
	assert(w != 0.0f);
	return result / w; // 演算子のオーバーライド
}

void Vector2::operator*=(const Matrix3x3 &matrix) {
	this->x = x * matrix.m[0][0] + y * matrix.m[1][0] + 1.0f * matrix.m[2][0];
	this->y = x * matrix.m[0][1] + y * matrix.m[1][1] + 1.0f * matrix.m[2][1];
	const float w = x * matrix.m[0][2] + y * matrix.m[1][2] + 1.0f * matrix.m[2][2];
	assert(w != 0.0f);
	*this / w; // 演算子のオーバーライド
}



Vector2::Vector2(float x, float y) : x(x), y(y)
{
}
