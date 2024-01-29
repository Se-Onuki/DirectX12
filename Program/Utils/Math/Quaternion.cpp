#include "Quaternion.h"
#include "Euler.h"
Quaternion const Quaternion::Identity{ Vector3::zero, 1.f };


Quaternion Quaternion::Create(const SoLib::Math::Euler &euler) {
	enum SinCos {
		kSin,
		kCos
	};

	const std::array<float, 2u> tX = { std::sin(euler.x / 2.f), std::cos(euler.x / 2.f) };
	const std::array<float, 2u> tY = { std::sin(euler.y / 2.f), std::cos(euler.y / 2.f) };
	const std::array<float, 2u> tZ = { std::sin(euler.z / 2.f), std::cos(euler.z / 2.f) };

	Quaternion result{
		/* x */ tX[kCos] * tY[kSin] * tZ[kSin] + tX[kSin] * tY[kCos] * tZ[kCos],
		/* y */ -tX[kSin] * tY[kCos] * tZ[kSin] + tX[kCos] * tY[kSin] * tZ[kCos],
		/* z */ tX[kCos] * tY[kCos] * tZ[kSin] + tX[kSin] * tY[kSin] * tZ[kCos],
		/* w */ -tX[kSin] * tY[kSin] * tZ[kSin] + tX[kCos] * tY[kCos] * tZ[kCos]
	};

	/*for (uint32_t i = 0u; i < 3u; i++) {
		Vector3 buff{};
		(&buff.x)[2u - i] = 1.f;
		result *= Quaternion::AnyAxisRotation(buff, (&euler.x)[i]);
	}*/

	return result.Normalize();
}