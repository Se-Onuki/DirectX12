/// @file Matrix.h
/// @brief 汎用的な行列
/// @author ONUKI seiya
#pragma once
#include <cstdint>

template<uint32_t Rows, uint32_t Columns = Rows>
struct Matrix {
	float m[Rows][Columns];

	static constexpr size_t Size = Rows * Columns;

	static size_t size() { return Size; }

	// beginメソッドの定義
	float *begin() {
		return &m[0][0];
	}
	// beginメソッドの定義
	const float *begin() const {
		return &m[0][0];
	}
	// beginメソッドの定義
	const float *cbegin() const {
		return &m[0][0];
	}

	// endメソッドの定義
	float *end() {
		return &m[0][0] + Size;
	}
	// endメソッドの定義
	const float *end() const {
		return &m[0][0] + Size;
	}
	// endメソッドの定義
	const float *cend() const {
		return &m[0][0] + Size;
	}

	template <uint32_t OtherColumns>
	Matrix<Rows, OtherColumns> operator*(const Matrix<Columns, OtherColumns> &other) const {
		Matrix<Rows, OtherColumns> result;

		for (uint32_t y = 0; y < Rows; ++y) {
			for (uint32_t x = 0; x < OtherColumns; ++x) {
				result.m[y][x] = 0;
				for (uint32_t k = 0; k < Columns; ++k) {
					result.m[y][x] += m[y][k] * other.m[k][x];
				}
			}
		}

		return result;
	}
};

template <uint32_t Rows, uint32_t Columns>
Matrix<Rows, Columns> operator+(const Matrix<Rows, Columns> &a, const Matrix<Rows, Columns> &b) {
	Matrix<Rows, Columns> result;
	for (uint32_t y = 0; y < Rows; ++y) {
		for (uint32_t x = 0; x < Columns; ++x) {
			result.m[y][x] = a.m[y][x] + b.m[y][x];
		}
	}
	return result;
}

template <uint32_t RowCloums, uint32_t OtherColumns>
Matrix<RowCloums> &operator*=(Matrix<RowCloums> &a, const Matrix<RowCloums, OtherColumns> &b) {
	return a = a * b;
}