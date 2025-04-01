/// @file Matrix4x4.h
/// @brief 4x4行列
/// @author ONUKI seiya
#pragma once
#include "Vector4.h"
#include <array>
#include <execution>
#include <immintrin.h>
namespace SoLib {

	struct Vector3;
	struct Vector3Norm;

	struct Matrix4x4 final
	{
		enum EulerAngle
		{
			Pitch, // x軸
			Yaw,   // y軸
			Roll   // z軸
		};

		Matrix4x4() = default;

		inline Matrix4x4(
			float A, float B, float C, float D, float E, float F, float G, float H, float I, float J,
			float K, float L, float M, float N, float O, float P)
			: vecs{
				  Vector4{A, B, C, D},
				  Vector4{E, F, G, H},
				  Vector4{I, J, K, L},
				  Vector4{M, N, O, P} } {
		}

		inline Matrix4x4(const Vector4 &A, const Vector4 &B, const Vector4 &C, const Vector4 &D)
		{
			vecs[0] = A;
			vecs[1] = B;
			vecs[2] = C;
			vecs[3] = D;
		}

		inline Matrix4x4(const std::array<Vector4, 4u> &vec)
		{
			vecs = vec;
		}

		union
		{
			std::array<Vector4, 4u> vecs;
			std::array<std::array<float, 4u>, 4u> m;
			std::array<float, 16u> arr;
		};

		/// @brief 逆行列関数
		/// @return 逆行列
		Matrix4x4 Inverse() const;

		/// @brief スケーリング無効の逆行列
		/// @return 逆行列
		Matrix4x4 InverseRT() const;

		/// @brief Transform逆行列
		/// @return 逆行列
		inline Matrix4x4 InverseSRT() const;

		/// @brief 転置行列関数
		/// @return 転置行列
		Matrix4x4 Transpose() const;

		Matrix4x4 operator+(const Matrix4x4 &Second) const;

		Matrix4x4 operator-(const Matrix4x4 &Second) const;

		inline Matrix4x4 operator*(const Matrix4x4 &Second) const;

		Matrix4x4 operator*(const float &Second) const;
		Matrix4x4 operator/(const float &Second) const;

		Matrix4x4 operator+=(const Matrix4x4 &Second);

		Matrix4x4 operator-=(const Matrix4x4 &Second);

		inline Matrix4x4 operator*=(const Matrix4x4 &Second);

		Matrix4x4 operator*=(const float &Second);
		Matrix4x4 operator/=(const float &Second);

		/// @brief 指定範囲を切り抜く関数
		/// @tparam row 行(y要素数)
		/// @tparam column 列(x要素数)
		/// @return 切り出した行列(その他は単位行列)
		template <uint8_t row, uint8_t column>
		Matrix4x4 Crop() const;
		/// @brief 回転行列を取得する
		Matrix4x4 GetRotate() const { return this->Crop<3u, 3u>(); }
		/// @brief 右ベクトル
		const Vector3 &GetRight() const { return vecs[0].ToVec3(); }
		/// @brief 上ベクトル
		const Vector3 &GetUp() const { return vecs[1].ToVec3(); }

		/// @brief 前方ベクトル
		const Vector3 &GetFront() const { return vecs[2].ToVec3(); }
		/// @brief 平行移動ベクトルを取得する
		/// @return 平行移動ベクトル
		Vector3 &GetTranslate() { return vecs[3].ToVec3(); }
		const Vector3 &GetTranslate() const { return vecs[3].ToVec3(); }

		/// @brief アフィン行列を生成する
		static Matrix4x4 Affine(const Vector3 &scale, const Vector3 &rotate, const Vector3 &translate);

		/// @brief 回転行列を生成する
		static Matrix4x4 EulerRotate(EulerAngle, float angle);
		/// @brief 回転行列を生成する
		static Matrix4x4 EulerRotate(const Vector3 &angle);

		/// @brief 任意軸の回転行列を生成する
		static Matrix4x4 AnyAngleRotate(const Vector3Norm &axis, const float angle);

		/// @brief 任意軸の回転行列を生成する
		static Matrix4x4 AnyAngleRotate(const Vector3Norm &axis, const float cos, const float sin);

		/// @brief 方向ベクトルから回転行列を生成する
		static Matrix4x4 DirectionToDirection(const Vector3Norm &from, const Vector3Norm &to);

		/// @brief 単位行列関数
		/// @return 単位行列
		static const Matrix4x4 &Identity()
		{
			static const Matrix4x4 identity{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
			return identity;
		}
		/// @brief 指定したベクトルを目指す視点行列を生成する
		static Matrix4x4 LookAtLH(
			const Vector3 &cameraPosition, // 視点の位置
			const Vector3 &cameraTarget,   // 目標の位置
			const Vector3 &cameraUpVector  // 上方を示すベクトル
		);

		static uint32_t size() { return 16u; }

		float *const begin() { return arr.data(); }
		const float *const begin() const { return arr.data(); }
		const float *const cbegin() const { return arr.data(); }

		float *const end() { return begin() + size(); }
		const float *const end() const { return end(); }
		const float *const cend() const { return end(); }

		float *const data() { return begin(); }
		const float *const data() const { return begin(); }
		const float *const cdata() const { return begin(); }

	private:
		/// @brief 行列の乗算
		inline static void Mul(Matrix4x4 &result, const Matrix4x4 &left, const Matrix4x4 &right);
	};

	bool operator==(const Matrix4x4 &left, const Matrix4x4 &right);

	bool operator!=(const Matrix4x4 &left, const Matrix4x4 &right);

#pragma region 4x4Func

	Matrix4x4 Matrix4x4::operator*(const Matrix4x4 &sec) const
	{
		Matrix4x4 result;
		Mul(result, *this, sec);
		return result;
	}

	Matrix4x4 Matrix4x4::operator*=(const Matrix4x4 &sec)
	{

		Mul(*this, *this, sec);
		return *this;
	}

	inline void Matrix4x4::Mul(Matrix4x4 &result, const Matrix4x4 &left, const Matrix4x4 &right)
	{
		const __m128 row0 = _mm_loadu_ps(right.m[0].data());
		const __m128 row1 = _mm_loadu_ps(right.m[1].data());
		const __m128 row2 = _mm_loadu_ps(right.m[2].data());
		const __m128 row3 = _mm_loadu_ps(right.m[3].data());
		for (int i = 0; i < 4; i++) {
			const __m128 xmm_all = _mm_loadu_ps(left.m[i].data());

			// 1つの__m128構造体から4つの__m128に分解
			const __m128 brod0 = _mm_permute_ps(xmm_all, 0x00);
			const __m128 brod1 = _mm_permute_ps(xmm_all, 0x55);
			const __m128 brod2 = _mm_permute_ps(xmm_all, 0xAA);
			const __m128 brod3 = _mm_permute_ps(xmm_all, 0xFF);
			const __m128 row = _mm_add_ps(
				_mm_add_ps(_mm_mul_ps(brod0, row0), _mm_mul_ps(brod1, row1)),
				_mm_add_ps(_mm_mul_ps(brod2, row2), _mm_mul_ps(brod3, row3)));
			_mm_storeu_ps(result.m[i].data(), row);
		}
	}

	Matrix4x4 Matrix4x4::InverseSRT() const
	{
		union VecSimd
		{
			Vector4 v;
			__m128 s;
			std::array<float, 4u> arr;

			VecSimd &operator=(const Vector4 &other)
			{
				v = other;
				return *this;
			}
		};
		std::array<VecSimd, 4> mm{
			VecSimd{.s = _mm_load_ps(vecs[0].begin())},
			VecSimd{.s = _mm_load_ps(vecs[1].begin())},
			VecSimd{.s = _mm_load_ps(vecs[2].begin())},
			VecSimd{.s = _mm_load_ps(vecs[3].begin())},
		};

		static const constinit int32_t mask = 0b01111111;

		const VecSimd vecX2 =
		{ .s = _mm_div_ps(mm[0].s, _mm_dp_ps(mm[0].s, mm[0].s, mask)) };
		const VecSimd vecY2 =
		{ .s = _mm_div_ps(mm[1].s, _mm_dp_ps(mm[1].s, mm[1].s, mask)) };
		const VecSimd vecZ2 =
		{ .s = _mm_div_ps(mm[2].s, _mm_dp_ps(mm[2].s, mm[2].s, mask)) };

		return Matrix4x4{
			{vecX2.v.x, vecY2.v.x, vecZ2.v.x, 0.f},
			{vecX2.v.y, vecY2.v.y, vecZ2.v.y, 0.f},
			{vecX2.v.z, vecY2.v.z, vecZ2.v.z, 0.f},
			{-_mm_cvtss_f32(_mm_dp_ps(mm[3].s, vecX2.s, mask)),
			 -_mm_cvtss_f32(_mm_dp_ps(mm[3].s, vecY2.s, mask)),
			 -_mm_cvtss_f32(_mm_dp_ps(mm[3].s, vecZ2.s, mask)), 1.f} };
	};
	template <uint8_t row, uint8_t column>
	inline Matrix4x4 Matrix4x4::Crop() const
	{
		Matrix4x4 result = Matrix4x4::Identity();

		for (uint32_t y = 0; y < row; ++y) {
			std::memcpy(result.m[y].data(), m[y].data(), sizeof(float) * column);
		}

		return result;
	}
	//
	///// @brief 文字列変換
	///// @param matrix
	///// @return
	// std::string SoLib::to_string(const Matrix4x4 &matrix) {
	//	std::string result = "[\n";
	//	for (uint32_t y = 0u; y < 4u; ++y) {
	//		result += "  [ ";
	//		for (uint32_t x = 0u; x < 4u; ++x) {
	//			result += std::to_string(matrix.m[y][x]) + ",\t";
	//			if (x < 3u) {
	//				result += ", ";
	//			}
	//		}
	//		result += " ],\n";
	//	}
	//	result += "]";
	//	return result;
	// }

#pragma endregion
}