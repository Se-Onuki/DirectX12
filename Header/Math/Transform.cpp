#include "Transform.h"
#include "Matrix4x4.h"

Matrix4x4 Transform::Affine() const
{
    return Matrix4x4::Affine(scale,rotate,translate);
}
