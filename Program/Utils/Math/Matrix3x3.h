#pragma once
#include <array>
#include <cstdint>
#include <immintrin.h>

struct Matrix3x3 {
	inline Matrix3x3() = default;

	inline Matrix3x3(float A, float B, float C, float D, float E, float F, float G, float H, float I) : m{ std::array<float, 3u>{A, B, C}, std::array<float, 3u>{D, E, F}, std::array<float, 3u>{G, H, I} } {	}

	using iterator = float *;
	using const_iterator = const float *;

	std::array<std::array<float, 3u>, 3u> m;

	//void Printf(const int &x, const int &y) const;

	/// <summary>
	/// 逆行列関数
	/// </summary>
	/// <returns>逆行列</returns>
	Matrix3x3 Inverse() const;

	/// <summary>
	/// 転置行列
	/// </summary>
	/// <returns>転置行列</returns>
	Matrix3x3 Transpose() const;

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

	inline iterator begin() { return m.data()->data(); }
	inline const_iterator begin() const { return m.data()->data(); }
	inline const_iterator cbegin() const { return m.data()->data(); }

	inline iterator end() { return begin() + size(); }
	const_iterator end() const { return end(); }
	const_iterator cend() const { return end(); }

	float *const data() { return m.data()->data(); }
	inline const float *data() const { return m.data()->data(); }
	inline const float *cdata() const { return m.data()->data(); }

private:

	// 転置をSIMDで高速化しようとしたけど、そもそもが速すぎて無駄
	/*inline static void TransponeArray(float data[8]) {
		static const __m256i indices = _mm256_set_epi32(5, 2, 7, 4, 1, 6, 3, 0);

		__m256 result = _mm256_permutevar8x32_ps(_mm256_loadu_ps(data), indices);

		_mm256_storeu_ps(data, result);
	}*/
};
