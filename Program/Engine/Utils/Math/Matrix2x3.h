/// @file Matrix2x3.h
/// @brief 2x3行列
/// @author ONUKI seiya
#pragma once
#include <valarray>
#include <array>
#include "SimdCalc.h"
namespace SoLib {

	struct alignas(16) Matrix2x3 {

		// 行列
		union {
			std::array<std::array<float, 2>, 3> m_;
			std::array<float, 2 * 3> vec_;
			std::array<SoLib::Math::SIMD128, 2> simd_;
		};
		std::array<float, 2> &operator[](size_t index) { return m_[index]; }
		const std::array<float, 2> &operator[](size_t index) const { return m_[index]; }

		static constexpr uint32_t size() { return 12u; }

		float *data() { return vec_.data(); }
		const float *data() const { return vec_.data(); }

		float *begin() { return vec_.data(); }
		const float *begin() const { return vec_.data(); }
		const float *cbegin() const { return vec_.data(); }
		float *end() { return vec_.data() + size(); }
		const float *end() const { return vec_.data() + size(); }
		const float *cend() const { return vec_.data() + size(); }

	};
}