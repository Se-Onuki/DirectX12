#pragma once
#include <cstdint>


struct Matrix3x3 {
	inline Matrix3x3() : m{ {0, 0, 0}, {0, 0, 0}, {0, 0, 0} } {	}

	inline Matrix3x3(float A, float B, float C, float D, float E, float F, float G, float H, float I) : m{ {A, B, C}, {D, E, F}, {G, H, I} } {	}

	float m[3][3];

	//void Printf(const int &x, const int &y) const;

	/// <summary>
	/// 逆行列関数
	/// </summary>
	/// <returns>逆行列</returns>
	Matrix3x3 Inverse();

	/// <summary>
	/// 転置行列
	/// </summary>
	/// <returns>転置行列</returns>
	Matrix3x3 Transpose();

	Matrix3x3 operator+(const Matrix3x3 &Second) const;

	Matrix3x3 operator-(const Matrix3x3 &Second) const;

	Matrix3x3 operator*(const Matrix3x3 &Second) const;


	Matrix3x3 operator*(const float &Second) const;
	Matrix3x3 operator/(const float &Second) const;



	Matrix3x3 operator+=(const Matrix3x3 &Second);

	Matrix3x3 operator-=(const Matrix3x3 &Second);

	Matrix3x3 operator*=(const Matrix3x3 &Second);


	Matrix3x3 operator*=(const float &Second);
	Matrix3x3 operator/=(const float &Second);


	/// <summary>
	/// 単位行列
	/// </summary>
	/// <returns>単位行列</returns>
	static Matrix3x3 Identity() {
		return Matrix3x3{
			1, 0, 0,
			0, 1, 0,
			0, 0, 1
		};
	}

	static uint32_t size() { return 9u; }

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
