#include "SoLib_Json.h"

namespace SoLib {

	template<>
	Vector2 cast(const nlohmann::json &jsonArray) {
		if (jsonArray.is_array() && jsonArray.size() == SoLib::Traits<Vector2>::Size) {
			// データの保存先
			Vector2 result;
			// データを書き込む
			for (uint32_t i = 0u; i < SoLib::Traits<decltype(result)>::Size; i++) {
				result.begin()[i] = jsonArray[i];
			}
			return result;
		}
		else {
			return Vector2::zero;
		}
	}

	template<>
	Vector3 cast(const nlohmann::json &jsonArray) {
		if (jsonArray.is_array() && jsonArray.size() == SoLib::Traits<Vector3>::Size) {
			// データの保存先
			Vector3 result;
			// データを書き込む
			for (uint32_t i = 0u; i < SoLib::Traits<decltype(result)>::Size; i++) {
				result.begin()[i] = jsonArray[i];
			}
			return result;
		}
		else {
			return Vector3::zero;
		}
	}

	template<>
	Vector4 cast(const nlohmann::json &jsonArray) {
		if (jsonArray.is_array() && jsonArray.size() == SoLib::Traits<Vector4>::Size) {
			// データの保存先
			Vector4 result;
			// データを書き込む
			for (uint32_t i = 0u; i < SoLib::Traits<decltype(result)>::Size; i++) {
				result.begin()[i] = jsonArray[i];
			}
			return result;
		}
		else {
			return Vector4::zero;
		}
	}
}