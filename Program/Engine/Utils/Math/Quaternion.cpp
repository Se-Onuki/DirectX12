/// @file Quaternion.cpp
/// @brief クォータニオン
/// @author ONUKI seiya
#include "Quaternion.h"
#include "Euler.h"
namespace SoLib {

	Quaternion const Quaternion::Identity{ Vector3::zero, 1.f };


	Quaternion Quaternion::Create(const SoLib::Math::Euler &euler) {
		enum SinCos {
			kSin,
			kCos
		};

		const std::array<float, 2u> tX = { std::sin(euler.x / 2.f), std::cos(euler.x / 2.f) };
		const std::array<float, 2u> tY = { std::sin(euler.y / 2.f), std::cos(euler.y / 2.f) };
		const std::array<float, 2u> tZ = { std::sin(euler.z / 2.f), std::cos(euler.z / 2.f) };

		// 回転の順序が逆
		//Quaternion result{
		//	/* x */ tX[kCos] * tY[kSin] * tZ[kSin] + tX[kSin] * tY[kCos] * tZ[kCos],
		//	/* y */ -tX[kSin] * tY[kCos] * tZ[kSin] + tX[kCos] * tY[kSin] * tZ[kCos],
		//	/* z */ tX[kCos] * tY[kCos] * tZ[kSin] + tX[kSin] * tY[kSin] * tZ[kCos],
		//	/* w */ -tX[kSin] * tY[kSin] * tZ[kSin] + tX[kCos] * tY[kCos] * tZ[kCos]
		//};

		Quaternion result{
			/* x */ tX[kSin] * tY[kCos] * tZ[kCos] - tX[kCos] * tY[kSin] * tZ[kSin],
			/* y */ tX[kSin] * tY[kCos] * tZ[kSin] + tX[kCos] * tY[kSin] * tZ[kCos],
			/* z */ -tX[kSin] * tY[kSin] * tZ[kCos] + tX[kCos] * tY[kCos] * tZ[kSin],
			/* w */ tX[kSin] * tY[kSin] * tZ[kSin] + tX[kCos] * tY[kCos] * tZ[kCos]
		};

		/*for (uint32_t i = 0u; i < 3u; i++) {
			Vector3 buff{};
			(&buff.x)[2u - i] = 1.f;
			result *= Quaternion::AnyAxisRotation(buff, (&euler.x)[i]);
		}*/

		return result.Normalize();
	}



	Quaternion &Quaternion::RK4(const Math::Euler &omega, float dt)
	{
		const Quaternion &q1 = *this;
		const Quaternion &om = Quaternion{ static_cast<const Vector3 &>(omega),0.f };
		Quaternion k1 = (q1 * om * 0.5f) * dt;

		Quaternion q2 = q1 + k1 * 0.5f;
		Quaternion k2 = (q2 * om * 0.5f) * dt;

		Quaternion q3 = q1 + k2 * 0.5f;
		Quaternion k3 = (q3 * om * 0.5f) * dt;

		Quaternion q4 = q1 + k3;
		Quaternion k4 = (q4 * om * 0.5f) * dt;

		*this = q1 + (k1 + k2 * 2.0f + k3 * 2.0f + k4) * (1.0f / 6.0f);
		Normalize();

		return *this;
	}
}