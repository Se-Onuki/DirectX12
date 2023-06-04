#include"Math.hpp"

#define _USE_MATH_DEFINES
#include<math.h>

#include "Vector2.h"
#include "Vector3.h"
#include "Polar.h"

#include <assert.h>
#include <numbers>



float DotProduct(Vector2 startA, Vector2 endA, Vector2 startB, Vector2 endB) {
	Vector2 a = { ((endA.x) - (startA.x)),((endA.y) - (startA.y)) };
	Vector2 b = { ((endB.x) - (startB.x)),((endB.y) - (startB.y)) };

	return a.x * b.x + a.y * b.y;
}

float DotProduct(Vector2 VectorA, Vector2 VectorB) {
	return VectorA.x * VectorB.x + VectorA.y * VectorB.y;
}


float CrossProduct(Vector2 startA, Vector2 endA, Vector2 startB, Vector2 endB) {
	Vector2 a = { ((endA.x) - (startA.x)),((endA.y) - (startA.y)) };
	Vector2 b = { ((endB.x) - (startB.x)),((endB.y) - (startB.y)) };

	return a.x * b.y - a.y * b.x;
}
float CrossProduct(Vector2 VectorA, Vector2 VectorB) {
	return VectorA.x * VectorB.y - VectorA.y * VectorB.x;
}



Vector2 operator <<(Vector2 &vec2, const Polar &Polar)
{
	return vec2 = {
		(float)(cos(Polar.theta) * Polar.radius), (float)(sin(Polar.theta) * Polar.radius)
	};
}

Polar operator<<(Polar &Polar, const Vector2 &vec2) {
	return Polar = { (float)(atan2(vec2.y, vec2.x)), vec2.Length() };
}




Matrix2x2 MakeRotateMatrix(const float &theta) {
	return Matrix2x2(
		cos(theta), sin(theta),
		-sin(theta), cos(theta)
	);
}


Matrix3x3 MakeScaleMatrix(const Vector2 &scale) {
	return Matrix3x3{
		scale.x, 0, 0,
		0, scale.y, 0,
		0, 0, 1
	};
}

Matrix4x4 MakeScaleMatrix(const Vector3 &scale) {
	return Matrix4x4{
		scale.x, 0, 0, 0,
		0, scale.y, 0, 0,
		0, 0, scale.z, 0,
		0, 0, 0, 1
	};
}



Matrix3x3 MakeRotateMatrix3x3(const float &theta) {
	return Matrix3x3(
		cos(theta), sin(theta), 0,
		-sin(theta), cos(theta), 0,
		0, 0, 1
	);
}

Matrix3x3 MakeTranslateMatrix(const Vector2 &translate) {
	return Matrix3x3{
		1, 0, 0,
		0, 1, 0,
		translate.x, translate.y, 1
	};
}

Matrix4x4 MakeTranslateMatrix(const Vector3 &translate) {
	return Matrix4x4{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		translate.x, translate.y, translate.z, 1
	};
}


Matrix3x3 MakeAffineMatrix(const Vector2 &scale, const float &theta, const Vector2 &translate) {
	return Matrix3x3{
		scale.x * (float)cos(theta) , scale.x * (float)sin(theta), 0,
		scale.y * (float)-sin(theta), scale.y * (float)cos(theta), 0,
		translate.x, translate.y, 1
	};
}


Vector2 Transform(const Vector2 &vector, const Matrix3x3 &matrix) {
	Vector2 result;

	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + 1.0f * matrix.m[2][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + 1.0f * matrix.m[2][1];
	const float w = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + 1.0f * matrix.m[2][2];
	assert(w != 0.0f);
	return result / w; // 演算子のオーバーライド
}


Vector3 Transform(const Vector3 &vector, const Matrix4x4 &matrix) {
	Vector3 result;

	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
	const float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];
	assert(w != 0.0f);
	return result / w; // 演算子のオーバーライド
}


Matrix3x3 MakeOrthographicMatrix(const Vector2 &LeftTop, const Vector2 &RightBottom) {
	return
	{
		2.0f / (RightBottom.x - LeftTop.x), 0,0,
		0, 2.0f / (LeftTop.y - RightBottom.y),0,
		(RightBottom.x + LeftTop.x) / (LeftTop.x - RightBottom.x), (LeftTop.y + RightBottom.y) / (RightBottom.y - LeftTop.y), 1
	};
}

Matrix3x3 MakeViewportMatrix(const Vector2 &LeftTop, const Vector2 &RightBottom) {
	return
	{
		(RightBottom.x - LeftTop.x) / 2.0f, 0,0,
		0, -(RightBottom.y - LeftTop.y) / 2.0f,0,
		(RightBottom.x - LeftTop.x) / 2.0f + LeftTop.x,(RightBottom.y - LeftTop.y) / 2.0f + LeftTop.y, 1
	};
}

float Angle::Digree::ToRadian() {
	return digree * Dig2Rad;
}

float Angle::Radian::ToDigree() {
	return radian * Rad2Dig;
}
