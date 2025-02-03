/// @file TransformMatrix.h
/// @brief Transform用のMatrix
/// @author ONUKI seiya
#pragma once
#include "Matrix3x3.h"
#include "Vector3.h"
#include "Euler.h"
#include <array>
#include <tuple>
#include <immintrin.h>
#include "../SoLib/SoLib_Traits.h"
namespace SoLib {

	struct Matrix4x4;

	struct TransformMatrix {
	public:
		//#pragma warning(push)  // 現在の警告のステータスを保存する
		//#pragma warning(disable : 4201)  // C4201警告を無視する

			// 無名共用体
		union {
			struct {
				Matrix3x3 rotMat;
				Vector3 translate;
			} transMat;
			std::array<std::array<float, 3u>, 4u> m;
			std::array<Vector3, 4u> vec;
			std::array<float, 12u> arr;
		};

		//#pragma warning(pop)  // 以前の警告のステータスに戻す

		static const TransformMatrix one;

		/// @brief 単位行列を取得する
		const TransformMatrix &Identity() const { return one; }

		/// @brief 逆行列を取得する
		TransformMatrix Inverse() const;

		/// @brief 逆行列を取得する(回転+平行移動行列のみ)
		TransformMatrix InverseRT() const;

		/// @brief 回転行列を取得する
		/// @return 回転行列
		Matrix3x3 &GetRotateMat() { return transMat.rotMat; }
		const Matrix3x3 &GetRotateMat() const { return transMat.rotMat; }

		/// @brief 右を示すベクトルを取得する
		/// @return 右ベクトル
		Vector3 &GetRightAxis() { return vec[0]; }
		const Vector3 &GetRightAxis() const { return vec[0]; }

		/// @brief 上を示すベクトルを取得する
		/// @return 上ベクトル
		Vector3 &GetUpAxis() { return vec[1]; }
		const Vector3 &GetUpAxis() const { return vec[1]; }

		/// @brief 前を示すベクトルを取得する
		/// @return 前ベクトル
		Vector3 &GetFrontAxis() { return vec[2]; }
		const Vector3 &GetFrontAxis() const { return vec[2]; }

		/// @brief 平行移動要素を取得する
		/// @return 平行移動要素
		Vector3 &GetTranslate() { return transMat.translate; }
		const Vector3 &GetTranslate() const { return transMat.translate; }

	public:

		/// @brief 行列を作成する
		static TransformMatrix MakeMatrix(const Matrix4x4 &matrix);

		/// @brief 回転行列を作成する
		static TransformMatrix MakeRotate(const SoLib::Math::Euler &rotate);

		/// @brief アフィン行列を作成する
		static TransformMatrix Affine(const Vector3 &scale, const SoLib::Math::Euler &rotate, const Vector3 &translate);
		/// @brief 任意軸での回転行列を作成する
		static TransformMatrix AnyAngleRotate(const Vector3Norm &axis, const float angle);
		static TransformMatrix AnyAngleRotate(const Vector3Norm &axis, const float cos, const float sin);
		/// @brief 特定の方向を向く行列を作成する
		static TransformMatrix DirectionToDirection(const Vector3Norm &from, const Vector3Norm &to);


	public:
		/// @brief 行列を乗算する
		inline static void Mul(TransformMatrix &result, const TransformMatrix &left, const TransformMatrix &right);
	};

	inline void TransformMatrix::Mul(TransformMatrix &result, const TransformMatrix &left, const TransformMatrix &right)
	{

		union VecSimd
		{
			__m128 simd;
			struct { Vector3 val; float pad; } vec;
			struct { std::array<float, 3u> val; float pad; } arr;
		};

		// 回転/スケール要素
		const std::array<VecSimd, 3u> rows{
			VecSimd {.arr = right.m[0]},
			VecSimd {.arr = right.m[1]},
			VecSimd {.arr = right.m[2]},
		};

		// 回転/スケール要素を乗算
		for (uint8_t i = 0; i < 4; i++) {
			const __m128 brod0 = _mm_set1_ps(left.m[i][0]);
			const __m128 brod1 = _mm_set1_ps(left.m[i][1]);
			const __m128 brod2 = _mm_set1_ps(left.m[i][2]);

			// 各要素に対してのドット積
			const VecSimd tmp{
				_mm_add_ps(
					_mm_add_ps(_mm_mul_ps(brod0, rows[0].simd), _mm_mul_ps(brod1, rows[1].simd)),
					_mm_mul_ps(brod2, rows[2].simd))
			};

			// データをコピー
			result.m[i] = tmp.vec.val;

		}

		// 平行移動成分を追加
		result.GetTranslate() += right.GetTranslate();

	}


	TransformMatrix operator*(const TransformMatrix &left, const TransformMatrix &right) {
		TransformMatrix result;

		TransformMatrix::Mul(result, left, right);

		return result;
	}

	TransformMatrix &operator*=(TransformMatrix &left, const TransformMatrix &right) {

		TransformMatrix::Mul(left, left, right);

		return left;

	}

	template<>
	struct SoLib::Traits<TransformMatrix> {
		using Type = TransformMatrix;
		static constexpr const char *const Name = "TransformMatrix";
		static constinit const uint32_t Rows = 4u;
		static constinit const uint32_t Columns = 3u;
		static constinit const uint32_t Size = Rows * Columns;
		using ElementType = float;

		static const ElementType *CBegin(const Type &data) { return data.arr.data(); }
		static const ElementType *End(const Type &data) { return CBegin(data) + Size; }
	};
}