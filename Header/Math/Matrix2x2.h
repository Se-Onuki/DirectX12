#pragma once


struct Matrix2x2 {
	Matrix2x2();
	Matrix2x2(float A);
	Matrix2x2(float A, float B, float C, float D);

	float m[2][2];

	//void Printf(const int& x, const int& y);

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
};
