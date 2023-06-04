#pragma once
template<typename T> inline T Lerp(const T &Start, const T &End, float t) {
	return Start + (End - Start) * t;
}
struct Vector3;

Vector3 Slerp(const Vector3 &Start, const Vector3 &End, float t);