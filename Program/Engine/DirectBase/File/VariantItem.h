/// @file VariantItem.h
/// @brief 値と名前を持つクラス
/// @author ONUKI seiya
#pragma once
#include "../../Engine/Utils/Text/StaticString.h"
#include "../../Engine/Utils/SoLib/SoLib_Traits.h"
#include <string>
#pragma warning(push)
#pragma warning(disable:4023)
#include "json.hpp"
#pragma warning(pop)

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

	/// @brief 値へのアクセス
	inline T *const operator->() { return &item_; }
	inline const T *const operator->() const { return &item_; }

	/// @brief 文字列を取得
	const char *const GetKey() const { return this->c_str(); }
	/// @brief 値を取得
	T &GetItem() { return item_; }
	const T &GetItem() const { return item_; }

	/// @brief 値を取得
	inline T &operator*() { return item_; }
	inline const T &operator*() const { return item_; }


private:
	T item_;
};

/// @brief jsonからVariantItemへの変換
template<SoLib::Text::ConstExprString Str, SoLib::IsRealType T>
inline void operator>>(const nlohmann::json &json, VariantItem<Str, T> &item) {
	item = json[item.GetKey()].get<T>();
}

/// @brief VariantItemへのjsonへの変換
template<SoLib::Text::ConstExprString Str, SoLib::IsRealType T>
inline nlohmann::json &operator<<(nlohmann::json &json, const VariantItem<Str, T> &item) {
	json[item.GetKey()] = item.GetItem();

	return json;
}

/// @brief VariantItemから型Tへの型変換
template<SoLib::Text::ConstExprString Str, SoLib::IsRealType T>
inline VariantItem<Str, T>::operator const T &() const noexcept {
	return item_;
}