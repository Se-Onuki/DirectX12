#pragma once
#include "../../Utils/SoLib/SoLib_Traits.h"
#include <string>
#include <json.hpp>

template <SoLib::IsNotPointer T>
class VariantItem final {
	VariantItem() = delete;
public:
	VariantItem(const std::string &key, const T &item = {}) : key_(key), item_(item) {}
	~VariantItem() = default;

	inline operator const T &() const noexcept;
	inline VariantItem &operator=(const T &item);

	inline VariantItem &operator=(const nlohmann::json &item);

	inline T *const operator->() { return &item_; }

	const std::string &GetKey() const { return key_; }
	T &GetItem() { return item_; }
	const T &GetItem() const { return item_; }


private:
	const std::string key_;
	T item_;
};

template<SoLib::IsNotPointer T>
inline VariantItem<T> &VariantItem<T>::operator=(const T &item) {
	item_ = item;
	return *this;
}

template<SoLib::IsNotPointer  T>
inline void operator>>(const nlohmann::json &json, VariantItem<T> &item) {
	item = json[item.GetKey()].get<T>();
}

template<SoLib::IsNotPointer  T>
inline nlohmann::json &operator<<(nlohmann::json &json, const VariantItem<T> &item) {
	json[item.GetKey()] = item.GetItem();

	return json;
}

template<SoLib::IsNotPointer  T>
inline VariantItem<T>::operator const T &() const noexcept {
	return item_;
}