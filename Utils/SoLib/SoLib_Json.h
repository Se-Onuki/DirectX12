#pragma once
#include "SoLib_Traits.h"
#include <variant>

#include "../../externals/nlohmann/json.hpp"

#include "../Math/Matrix2x2.h"
#include "../Math/Matrix3x3.h"
#include "../Math/Matrix4x4.h"
#include "../Math/Quaternion.h"
#include "../Math/Vector2.h"
#include "../Math/Vector3.h"
#include "../Math/Vector4.h"

namespace SoLib {

	template<SoLib::IsNotPointer T>
	using JsonItemPtr = std::variant<bool T:: *, int32_t T:: *, float T:: *, std::string T:: *, Vector2 T:: *, Vector3 T:: *, Vector4 T:: *, Quaternion T:: * >;

	template<SoLib::IsNotPointer T>
	using JsonPair = std::list<std::pair<std::string, std::list<JsonItemPtr<T>>>>;

	template<SoLib::IsNotPointer T>
	inline JsonPair<T> GetJsonPair();

	template<>
	inline JsonPair<Quaternion> GetJsonPair<Quaternion>() {
		JsonPair<Quaternion> result;
		result.push_back({ "vec", { reinterpret_cast<Vector3 Quaternion:: *>(&Quaternion::x) } });
		result.push_back({ "w", { &Quaternion::w } });
		return result;
	}


	template<SoLib::IsNotPointer T>
	inline void ToJson(nlohmann::json &json, const T &data);

	template<SoLib::IsNotPointer T>
	inline void FromJson(const nlohmann::json &json, T &data);

}

void from_json(const nlohmann::json &json, Vector2 &data);
void from_json(const nlohmann::json &json, Vector3 &data);
void from_json(const nlohmann::json &json, Vector4 &data);

void to_json(nlohmann::json &json, const Quaternion &data);
void from_json(const nlohmann::json &json, Quaternion &data);

#pragma region Func

namespace SoLib {

	template<SoLib::IsNotPointer T>
	inline void ToJson(nlohmann::json &json, const T &data) {
		static const auto &jsonPair = GetJsonPair<T>();

		for (const auto &[key, ptrList] : jsonPair) {
			if (ptrList.size() == 1u) {

				auto &jsonItem = json[key];
				std::visit([&jsonItem, &data](auto T:: *const ptrv)
					{
						jsonItem = data.*ptrv;
					}, ptrList.front());
			}
			else {
				for (const auto &member : ptrList) {

					auto &jsonItem = json[key];
					std::visit([&jsonItem, &data](auto T:: *const ptrv)
						{
							jsonItem.push_back(data.*ptrv);
						}, member);

				}
			}

		}
	}

	template<SoLib::IsNotPointer T>
	inline void FromJson(const nlohmann::json &json, T &data) {
		const auto &jsonPair = GetJsonPair<T>();

		for (const auto &[key, ptrList] : jsonPair) {
			if (ptrList.size() == 1u) {

				const auto &jsonItem = json.at(key);
				std::visit([&jsonItem, &data](auto T:: *const ptrv)
					{
						data.*ptrv = jsonItem;
					}, ptrList.front());

			}
			else {
				uint32_t index = 0u;
				for (const auto &member : ptrList) {
					const auto &jsonItem = json.at(key).at(index);
					std::visit([&jsonItem, &data](auto T:: *const ptrv)
						{
							data.*ptrv = jsonItem;
						}, member);
					index++;
				}
			}
		}
	}

}

#pragma endregion