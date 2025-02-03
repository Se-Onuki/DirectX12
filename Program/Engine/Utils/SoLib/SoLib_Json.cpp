/// @file SoLib_Json.cpp
/// @brief Jsonへの変換
/// @author ONUKI seiya
#include "SoLib_Json.h"
namespace SoLib {
	void from_json(const nlohmann::json &json, SoLib::Vector2 &data) {
		if (json.is_array() && json.size() == SoLib::Traits<SoLib::Vector2>::Size) {
			// データを書き込む
			for (uint32_t i = 0u; i < SoLib::Traits<SoLib::Vector2>::Size; i++) {
				data.begin()[i] = json.at(i);
			}
		}
	}

	void from_json(const nlohmann::json &json, SoLib::Vector3 &data) {
		if (json.is_array() && json.size() == SoLib::Traits<SoLib::Vector3>::Size) {
			// データを書き込む
			for (uint32_t i = 0u; i < SoLib::Traits<SoLib::Vector3>::Size; i++) {
				data.begin()[i] = json.at(i);
			}
		}
	}

	void from_json(const nlohmann::json &json, SoLib::Vector4 &data) {
		if (json.is_array() && json.size() == SoLib::Traits<SoLib::Vector4>::Size) {
			// データを書き込む
			for (uint32_t i = 0u; i < SoLib::Traits<SoLib::Vector4>::Size; i++) {
				data.begin()[i] = json.at(i);
			}
		}
	}
	void to_json(nlohmann::json &json, const SoLib::Quaternion &data) {
		using Data = SoLib::Traits<SoLib::Quaternion>;
		if (json.is_array() && json.size() == Data::Size) {
			// データを書き込む
			for (uint32_t i = 0u; i < Data::Size; i++) {
				json.at(i) = Data::CBegin(data)[i];
			}
		}
	}

	void from_json(const nlohmann::json &json, SoLib::Quaternion &data) {
		using Data = SoLib::Traits<SoLib::Quaternion>;
		if (json.is_array() && json.size() == Data::Size) {
			// データを書き込む
			for (uint32_t i = 0u; i < Data::Size; i++) {
				(&data.x)[i] = json.at(i);
			}
		}
	}
}