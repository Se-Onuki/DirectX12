#include "SoLib_Json.h"

void from_json(const nlohmann::json &json, Vector2 &data) {
	if (json.is_array() && json.size() == SoLib::Traits<Vector2>::Size) {
		// データを書き込む
		for (uint32_t i = 0u; i < SoLib::Traits<Vector2>::Size; i++) {
			data.begin()[i] = json.at(i);
		}
	}
}

void from_json(const nlohmann::json &json, Vector3 &data) {
	if (json.is_array() && json.size() == SoLib::Traits<Vector3>::Size) {
		// データを書き込む
		for (uint32_t i = 0u; i < SoLib::Traits<Vector3>::Size; i++) {
			data.begin()[i] = json.at(i);
		}
	}
}

void from_json(const nlohmann::json &json, Vector4 &data) {
	if (json.is_array() && json.size() == SoLib::Traits<Vector4>::Size) {
		// データを書き込む
		for (uint32_t i = 0u; i < SoLib::Traits<Vector4>::Size; i++) {
			data.begin()[i] = json.at(i);
		}
	}
}
void to_json(nlohmann::json &json, const Quaternion &data) {
	SoLib::ToJson(json, data);
}

void from_json(const nlohmann::json &json, Quaternion &data) {
	SoLib::FromJson(json, data);
}