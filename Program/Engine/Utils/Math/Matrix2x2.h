#pragma once
#include <cstdint>


struct Matrix2x2 {
	Matrix2x2();
	Matrix2x2(float A);
	Matrix2x2(float A, float B, float C, float D);

	float m[2][2];

	//void Printf(const int32_t  x, const int32_t  y);

	Matrix2x2 operator+(const Matrix2x2& Second) const;
	Matrix2x2 operator-(const Matrix2x2& Second) const;


	Matrix2x2 operator*(const Matrix2x2& Second)const;


	Matrix2x2 operator*(const float& Second) const;
	Matrix2x2 operator/(const float& Second) const;


	void operator+=(const Matrix2x2& Second);
	void operator-=(const Matrix2x2& Second);


	void operator*=(const Matrix2x2& Second);


	void operator*=(const float& Second);
	void operator/=(const float& Second);

	static uint32_t size() { return 4u; }

	float *const begin() { return *m; }
	const float *const begin() const { return *m; }
	const float *const cbegin() const { return *m; }

	float *const end() { return begin() + size(); }
	const float *const end() const { return end(); }
	const float *const cend() const { return end(); }

	float *const data() { return begin(); }
	const float *const data() const { return begin(); }
	const float *const cdata() const { return begin(); }
};
