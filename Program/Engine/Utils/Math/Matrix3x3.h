/// @file Matrix3x3.h
/// @brief 3x3行列
/// @author ONUKI seiya
#pragma once
#include <array>
#include <cstdint>
#include <immintrin.h>
#include "Vector3.h"

namespace SoLib {

	struct Matrix3x3 {
		inline Matrix3x3() = default;

		inline Matrix3x3(float A, float B, float C, float D, float E, float F, float G, float H, float I) : m{ std::array<float, 3u>{A, B, C}, std::array<float, 3u>{D, E, F}, std::array<float, 3u>{G, H, I} } {	}

		using iterator = float *;
		using const_iterator = const float *;

#pragma warning(push)  // 現在の警告のステータスを保存する
#pragma warning(disable : 4201)  // C4201警告を無視する

		// 無名共用体
		union {
			std::array<float, 9u> arr;
			std::array<std::array<float, 3u>, 3u> m;
			std::array<Vector3, 3u> vec;
		};

#pragma warning(pop)  // 以前の警告のステータスに戻す

		//void Printf(const int32_t x, const int32_t y) const;

		/// @brief 逆行列関数
		/// @return 逆行列
		Matrix3x3 Inverse() const;

		/// @brief 転置行列
		/// @return 転置行列
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
}