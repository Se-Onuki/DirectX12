#include "SoLib_Json.h"
#include <variant>

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
using JsonItemPtr = std::variant<int32_t T:: *, float T:: *, std::string T:: *, Vector2 T:: *, Vector3 T:: *, Vector4 T:: *, Quaternion T:: *>;

template<SoLib::IsNotPointer T>
struct JsonPairStruct {
	enum class TypeName {
		kChar,
		kInt,
		kFloat,
	};

	std::string key;
	std::list<JsonItemPtr<T>> ptrList;
	TypeName typeName = TypeName::kFloat;
};

template<SoLib::IsNotPointer T>
using JsonPair = std::list<JsonPairStruct<T>>;

template<SoLib::IsNotPointer T>
JsonPair<T> GetJsonPair();

template<>
JsonPair<Quaternion> GetJsonPair<Quaternion>() {
	static JsonPair<Quaternion> result;
	if (not result.size()) {
		result.push_back(JsonPairStruct<Quaternion>{ "vec", { reinterpret_cast<Vector3 Quaternion:: *>(&Quaternion::x) }});
		result.push_back(JsonPairStruct<Quaternion>{ "w", { &Quaternion::w } });
	}
	return result;
}

void to_json(nlohmann::json &json, const Quaternion &data) {
	const auto &jsonPair = GetJsonPair<Quaternion>();

	for (const auto &pairData : jsonPair) {
		const auto &key = pairData.key;
		const auto &ptrList = pairData.ptrList;
		if (ptrList.size() == 1u) {

			auto &jsonItem = json[key];
			std::visit([&jsonItem, &data](auto Quaternion:: *const ptrv)
				{
					jsonItem = data.*ptrv;
				}, ptrList.front());
		}
		else {
			for (const auto &member : ptrList) {

				auto &jsonItem = json[key];
				std::visit([&jsonItem, &data](auto Quaternion:: *const ptrv)
					{
						jsonItem.push_back(data.*ptrv);
					}, member);

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

			const auto &jsonItem = json.at(key);
			std::visit([&jsonItem, &data](auto Quaternion:: *const ptrv)
				{
					data.*ptrv = jsonItem;
				}, ptrList.front());

		}
		else {
			uint32_t index = 0u;
			for (const auto &member : ptrList) {
				const auto &jsonItem = json.at(key).at(index);
				std::visit([&jsonItem, &data](auto Quaternion:: *const ptrv)
					{
						data.*ptrv = jsonItem;
					}, member);
				index++;
			}
		}
	}
}