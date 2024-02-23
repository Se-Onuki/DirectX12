#include "TransformMatrix.h"
#include "Math.hpp"

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
