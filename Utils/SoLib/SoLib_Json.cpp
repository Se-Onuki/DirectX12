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


template<SoLib::IsNotPointer T>
struct JsonPairStruct {
	enum class TypeName {
		kChar,
		kInt,
		kFloat,
	};

	std::string key;
	std::list<float T:: *> ptrList;
	TypeName typeName = TypeName::kFloat;
};

template<SoLib::IsNotPointer T>
using JsonPair = std::list<JsonPairStruct<T>>;

template<SoLib::IsNotPointer T>
JsonPair<T> GetJsonPair();

template<>
JsonPair<Quaternion> GetJsonPair<Quaternion>() {
	JsonPair<Quaternion> result;

	result.push_back(JsonPairStruct<Quaternion>{ "vec", { &Quaternion::x, &Quaternion::y, &Quaternion::z } });
	result.push_back(JsonPairStruct<Quaternion>{ "w", { &Quaternion::w } });

	return result;
}

void to_json(nlohmann::json &json, const Quaternion &data) {
	const auto &jsonPair = GetJsonPair<Quaternion>();

	for (const auto &pairData : jsonPair) {
		const auto &key = pairData.key;
		const auto &ptrList = pairData.ptrList;
		if (ptrList.size() == 1u) {
			json[key] = data.*ptrList.front();
		}
		else {
			for (const auto &member : ptrList) {
				json[key].push_back(data.*member);
			}
		}

	}
}

void from_json(const nlohmann::json &json, Quaternion &data) {
	const auto &jsonPair = GetJsonPair<Quaternion>();

	for (const auto &pairData : jsonPair) {
		const auto &key = pairData.key;
		const auto &ptrList = pairData.ptrList;
		if (ptrList.size() == 1u) {
			data.*ptrList.front() = json.at(key);
		}
		else {
			uint32_t index = 0u;
			for (const auto &member : ptrList) {
				data.*member = json.at(key).at(index);
				index++;
			}
		}
	}
}