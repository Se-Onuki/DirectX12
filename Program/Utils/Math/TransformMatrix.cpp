#include "TransformMatrix.h"
#include "Math.hpp"

const TransformMatrix TransformMatrix::one = TransformMatrix{ Matrix3x3::Identity() };

TransformMatrix TransformMatrix::Inverse() const
{
	TransformMatrix result;

	result.GetRotateMat() = GetRotateMat().Inverse();
	return result;
}

TransformMatrix TransformMatrix::InverseRT() const
{
	TransformMatrix result;

	result.GetRotateMat() = GetRotateMat().Transpose();

	result.GetTranslate() = GetTranslate() * result.GetRotateMat();
	return result;
}

TransformMatrix operator*(const TransformMatrix &left, const TransformMatrix &right) {
	TransformMatrix result;
	Matrix3x3 transpone = right.rotMat.Transpose();

	for (uint8_t i = 0u; i < 3u; i++) {
		for (uint8_t j = 0u; j < 4u; j++) {
			result.rotMat.vec[i].arr[j] = left.vec[i] * transpone.vec[j];
		}
	}
	for (uint8_t i = 0u; i < 3u; i++) {
		result.translate.arr[i] = left.translate * transpone.vec[i];
	}

	return result;
}
