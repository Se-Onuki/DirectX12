#pragma once
#include "../../Engine/Utils/Text/StaticString.h"
#include "../../Engine/Utils/SoLib/SoLib_Traits.h"
#include <string>
#pragma warning(push)
#pragma warning(disable:4023)
#include "json.hpp"
#pragma warning(pop)

/// 
#define VItem(T, Name) VariantItem<#Name,T> v##Name
/// 修飾文字付き
#define VItem(T, Name, Deco) VariantItem<#Name,T> v##Name##Deco

template<SoLib::Text::ConstExprString Str, SoLib::IsRealType T>
class VariantItem final : public SoLib::Text::StaticString<Str> {
public:
	VariantItem() = default;
	VariantItem(const T &item) : item_(item) {}
	VariantItem(T &&item) : item_(std::move(item)) {}
	~VariantItem() = default;

	using ItemType = T;

	inline operator const T &() const noexcept;
	inline VariantItem &operator=(const T &item) { item_ = item; return *this; }
	inline VariantItem &operator=(T &&item) { item_ = std::move(item); return *this; }

	inline T *const operator->() { return &item_; }
	inline const T *const operator->() const { return &item_; }

	const char *const GetKey() const { return this->c_str(); }
	T &GetItem() { return item_; }
	const T &GetItem() const { return item_; }

	inline T &operator*() { return item_; }
	inline const T &operator*() const { return item_; }


private:
	T item_;
};

template<SoLib::Text::ConstExprString Str, SoLib::IsRealType T>
inline void operator>>(const nlohmann::json &json, VariantItem<Str, T> &item) {
	item = json[item.GetKey()].get<T>();
}

template<SoLib::Text::ConstExprString Str, SoLib::IsRealType T>
inline nlohmann::json &operator<<(nlohmann::json &json, const VariantItem<Str, T> &item) {
	json[item.GetKey()] = item.GetItem();

	return json;
}

template<SoLib::Text::ConstExprString Str, SoLib::IsRealType T>
inline VariantItem<Str, T>::operator const T &() const noexcept {
	return item_;
}