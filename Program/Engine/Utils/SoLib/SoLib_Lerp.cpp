/// @file SoLib_Lerp.cpp
/// @brief Lerpの実装
/// @author ONUKI seiya
#include "SoLib_Lerp.h"

#include "../Math/Vector3.h"
#include "../Math/Transform.h"
#include <algorithm>
#include <cassert>

namespace SoLib {

	Vector3 Slerp(const Vector3 &Start, const Vector3 &End, float t) {
		if (Start.LengthSQ() == 0.f || End.LengthSQ() == 0.f) { // 片方の長さが0であった場合、「角度」が存在しないので通常の線形補間を行う。
			return Lerp(Start, End, t);
		}

		Vector3 nStart = Start.Nomalize();
		Vector3 nEnd = End.Nomalize();

		float angle = nStart * nEnd;

		angle = std::clamp(angle, -1.f, 1.f);	// 計算上の誤差を-1.f~1.fの間に丸める
		const float theta = std::acos(angle);	// 丸めないとnan(-ind)が返ってエラーが発生する。StartとEndの間の角度
		const float LerpLength = Lerp(Start.Length(), End.Length(), t);	// ベクトル長のみの線形補間

		const float sinTheta = std::sin(theta);	// 正規化後においてのEndに投影したStartベクトルの高さ

		if (sinTheta == 0.f) { // ゼロ除算対策。 
			return LerpLength * nStart;
		}
		// nStartとnEndが同じ == 角度の差がない == 距離だけの線形補間でよい。
		return LerpLength * (std::sin((1 - t) * theta) * nStart + std::sin(t * theta) * nEnd) / sinTheta;
	}


	uint32_t ColorLerp(uint32_t statColor, uint32_t endColor, float easingVolume) {


		/*std::array<int16_t, 4u> intStart{};
		std::array<int16_t, 4u> intEnd;
		for (uint8_t i = 0; i < 4; i++) {
			intStart[i] = static_cast<int16_t>((statColor >> (8u * (i))) & 0xFF);
			intEnd[i] = static_cast<int16_t>((endColor >> (8u * (i))) & 0xFF);
		}*/

		std::array<uint8_t, 4u> result{};

		for (uint8_t i = 0u; i < 4u; i++) {

			result[i] = static_cast<uint8_t>(std::lerp<float,float,float>((*reinterpret_cast<std::array<int8_t, 4u> *>(&statColor))[i], (*reinterpret_cast<std::array<int8_t, 4u> *>(&endColor))[i], easingVolume));
		}


		return *reinterpret_cast<uint32_t *>(&result);
	}

}