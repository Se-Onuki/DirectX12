#pragma once
#include <immintrin.h>
#include <cstdint>
#include <smmintrin.h>

namespace SoLib {

	namespace Math {

		struct SIMD128 {
			SIMD128() = default;
			SIMD128(const __m128 &other) : value_(other) {};
			SIMD128(__m128 &&other) : value_(other) {};

			inline operator __m128 &() noexcept { return value_; }
			inline operator const __m128 &() const noexcept { return value_; }

			template<uint8_t Length>
			static inline float Dot(const __m128 &vecA, const __m128 &vecB);

			/// @brief Vector3型のSIMDを用いたクロス積
			/// @param vecA ベクトルA
			/// @param vecB ベクトルB
			/// @return Vector3型のSIMD
			static inline SIMD128 Cross(const __m128 &vecA, const __m128 &vecB);

			inline SIMD128 operator+(const SIMD128 &other) const;
			inline SIMD128 operator-(const SIMD128 &other) const;
			inline SIMD128 operator*(const float other) const;
			inline SIMD128 operator/(const float other) const;

			inline SIMD128 operator-()const { return *this * -1.f; }

		private:

			template <uint8_t Num>
			struct BitPattern {
				static constexpr int32_t value = (1 << (Num + 4)) - 1;
			};

			__m128 value_;
		};
	}
	using namespace Math;
}

template<uint8_t Length>
inline float SoLib::Math::SIMD128::Dot(const __m128 &vecA, const __m128 &vecB) {
	return _mm_cvtss_f32(_mm_dp_ps(vecA, vecB, BitPattern<Length>::value));
}

///
/// A * B = 
/// {   p0   p1   p2   p3
///     Ay * Bz - Az * By,
///     Az * Bx - Ax * Bz,
///     Ax * By - Ay * Bx
/// };
///

SoLib::Math::SIMD128 SoLib::Math::SIMD128::Cross(const __m128 &vecA, const __m128 &vecB) {

	// vecAから、(y, z, x, w)を取得する (p0)
	__m128 p0 = _mm_shuffle_ps(vecA, vecA, _MM_SHUFFLE(3, 0, 2, 1));
	// vecBから、(z, x, y, w)を取得する (p1)
	__m128 p1 = _mm_shuffle_ps(vecB, vecB, _MM_SHUFFLE(3, 1, 0, 2));

	// p0とp1の各要素を乗算。           (p0 * p1)
	__m128 mul0 = _mm_mul_ps(p0, p1);

	// p0とvecBの各要素を乗算。         (p0 * vecB) == (p2' * p3')
	// p2とp3を、p3のxyz順に並べると、p2とp0の順序が一致する。
	// (Ay * Bx, Az * By, Ax * Bz, Aw * Bw) 
	__m128 buff = _mm_mul_ps(p0, vecB);
	// p2*p3の順序を正しくする。
	// vecB(x, y, z)の順をp3(y, z, x)の順に。
	__m128 mul1 = _mm_shuffle_ps(buff, buff, _MM_SHUFFLE(3, 0, 2, 1));

	// p0*p1 - p2*p3を計算する。
	return SoLib::Math::SIMD128{ _mm_sub_ps(mul0, mul1) };
}

inline SoLib::Math::SIMD128 SoLib::Math::SIMD128::operator+(const SIMD128 &other) const {
	return SoLib::Math::SIMD128{ _mm_add_ps(*this,other) };
}

inline SoLib::Math::SIMD128 SoLib::Math::SIMD128::operator-(const SIMD128 &other) const {
	return SoLib::Math::SIMD128{ _mm_sub_ps(*this,other) };
}

inline SoLib::Math::SIMD128 SoLib::Math::SIMD128::operator*(const float other) const {
	return SoLib::Math::SIMD128{ _mm_mul_ps(*this, _mm_load_ps1(&other)) };
}

inline SoLib::Math::SIMD128 SoLib::Math::SIMD128::operator/(const float other) const {
	return SoLib::Math::SIMD128{ _mm_div_ps(*this, _mm_load_ps1(&other)) };
}

//inline SoLib::Math::SIMD128 operator+ (const SoLib::Math::SIMD128 &a, const SoLib::Math::SIMD128 &b) {
//
//	return SoLib::Math::SIMD128{ _mm_add_ps(a,b) };
//
//}
//
//inline SoLib::Math::SIMD128 operator* (const SoLib::Math::SIMD128 &a, float b) {
//
//	return SoLib::Math::SIMD128{ _mm_mul_ps(a, _mm_load_ps1(&b)) };
//
////}
//inline SoLib::Math::SIMD128 operator/ (const SoLib::Math::SIMD128 &a, float b) {
//
//	return SoLib::Math::SIMD128{ _mm_div_ps(a, _mm_load_ps1(&b)) };
//
//}