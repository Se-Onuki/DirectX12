#pragma once
#include "../../Engine/Utils/Math/Vector2.h"
#include "../../Engine/Utils/Math/Vector3.h"
#include "../../Engine/Utils/Math/Vector4.h"
#include "../../Engine/Utils/Math/Angle.h"
#include <assert.h>
#include <stdint.h>
#include <string>
#include <unordered_map>
#include <variant>

#include "VariantItem.h"

class GlobalVariables {
	GlobalVariables() = default;
	GlobalVariables(const GlobalVariables &) = delete;
	GlobalVariables operator=(const GlobalVariables &) = delete;
	~GlobalVariables() = default;

	const std::string kDirectoryPath = "resources/GlobalVariables/";

public:
	/// @brief 保存できる型
	using Item = std::variant<int32_t, float, Vector2, Vector3, Vector4, Angle::Radian, std::string>;
	/// @brief 1つのグループに保存する連想記憶コンテナ
	using Group = std::unordered_map<std::string, Item>;

	static GlobalVariables *const GetInstance() {
		static GlobalVariables instance{};
		return &instance;
	}

	void CreateGroups(const std::string &groupName) { datas_[groupName]; }

	/// @brief Group自体のゲッタ
	/// @param groupName グループ名
	/// @return 紐づいた Group
	const Group &GetGroup(const std::string &groupName) const;
	Group &GetGroup(const std::string &groupName);

	Group &operator[](const std::string &groupName) { return datas_[groupName]; }

	/// @brief Item自体のゲッタ
	/// @param groupName グループ名
	/// @param key 紐づけられたキー
	/// @return 紐づいた Item
	const Item &Get(const std::string &groupName, const std::string &key) const;

	/// @brief 変換付きゲッタ
	/// @tparam T 変換先の型
	/// @param groupName グループ名
	/// @param key 紐づけられたキー
	/// @return 型変換した値
	template<typename T>
	T Get(const std::string &groupName, const std::string &key) const;
	/*template<typename T>
	void AddValue(const std::string& groupName, const std::string& key, const T value);*/

	template<typename T>
	void AddValue(const std::string &groupName, const std::string &key, const T &value);

	template<typename T>
	void SetValue(const std::string &groupName, const std::string &key, const T &value);

	template <typename ITEM, SoLib::Text::ConstExprString V = ITEM::str_, SoLib::IsRealType T = typename ITEM::ItemType>
		requires(std::same_as<ITEM, VariantItem<V, T>>)
	void AddValue(const std::string &groupName, const ITEM &item);

	template <typename ITEM, SoLib::Text::ConstExprString V = ITEM::str_, SoLib::IsRealType T = typename ITEM::ItemType>
		requires(std::same_as<ITEM, VariantItem<V, T>>)
	void SetValue(const std::string &groupName, const ITEM &item);

	void Update();

	void SaveFile(const std::string &groupName) const;

	void LoadFile();
	void LoadFile(const std::string &groupName);

private:
	std::unordered_map<std::string, Group> datas_;
};

/// @brief Itemから型変換を行って代入
/// @tparam T 代入する型
/// @param value 代入先の変数
/// @param item 保存されたItem
template<typename T>
void operator>> (const GlobalVariables::Item &item, T &value) {

	// 型が正しいか
	if (std::holds_alternative<T>(item)) {
		value = std::get<T>(item);
		return;
	}
	assert(0 && "存在しない型を取得しようとしました。");
	value = T{};
}

/// @brief Itemから型変換を行って代入
/// @tparam T 代入する型
/// @param value 代入先の変数
/// @param item 保存されたItem

template <typename ITEM, SoLib::Text::ConstExprString V = ITEM::str_, SoLib::IsRealType T = typename ITEM::ItemType>
	requires(std::same_as<ITEM, VariantItem<V, T>>)
void operator>> (const GlobalVariables::Item &item, ITEM &value) {

	// 型が正しいか
	if (std::holds_alternative<T>(item)) {
		value = std::get<T>(item);
		return;
	}
	assert(0 && "存在しない型を取得しようとしました。");
	value = T{};
}

template <typename ITEM, SoLib::Text::ConstExprString V = ITEM::str_, SoLib::IsRealType T = typename ITEM::ItemType>
	requires(std::same_as<ITEM, VariantItem<V, T>>)
void operator>> (const GlobalVariables::Group &group, ITEM &value) {

	// キーがあるか
	const auto &itItem = group.find(value.GetKey());
	//assert(itItem != group.end());
	if (itItem == group.end()) { return; }

	// アイテムの参照
	const auto &item = itItem->second;

	// 型が正しいか
	if (std::holds_alternative<T>(item)) {
		value = std::get<T>(item);
		return;
	}
	assert(0 && "存在しない型を取得しようとしました。");
	value = T{};
}

template <typename ITEM, SoLib::Text::ConstExprString V = ITEM::str_, SoLib::IsRealType T = typename ITEM::ItemType>
	requires(std::same_as<ITEM, VariantItem<V, T>>)
void operator<< (GlobalVariables::Group &group, const ITEM &value) {
	group[value.GetKey()] = value.GetItem();
}

//
//template<typename T>
//void operator<< (GlobalVariables::Group &group, VariantItem<T> &value) {
//
//	// キーがあるか
//	const auto &itItem = group.find(value.GetKey());
//	assert(itItem != group.end());
//
//	// アイテムの参照
//	auto &item = itItem->second;
//
//	item = value.GetItem();
//}

template<typename T>
inline T GlobalVariables::Get(const std::string &groupName, const std::string &key) const {

	// アイテムの検索
	const Item &item = Get(groupName, key);
	// 型が正しいか
	if (std::holds_alternative<T>(item)) {
		return std::get<T>(item);
	}
	assert(0 && "存在しない型を取得しようとしました。");
	return T{};
}

template<typename T>
void GlobalVariables::AddValue(
	const std::string &groupName, const std::string &key, const T &value) {
	auto itItem = datas_[groupName].find(key); // キーがあるか
	if (itItem == datas_[groupName].end()) {
		SetValue(groupName, key, value);
		return;
	}
	return;
}

template<typename T>
void GlobalVariables::SetValue(
	const std::string &groupName, const std::string &key, const T &value) {
	Group &group = datas_[groupName];
	group[key] = value;
}

template <typename ITEM, SoLib::Text::ConstExprString V,  SoLib::IsRealType T>
	requires(std::same_as<ITEM, VariantItem<V, T>>)
inline void GlobalVariables::AddValue(const std::string &groupName, const ITEM &item) {
	AddValue(groupName, item.GetKey(), item.GetItem());
}

template <typename ITEM, SoLib::Text::ConstExprString V, SoLib::IsRealType T>
	requires(std::same_as<ITEM, VariantItem<V, T>>)
inline void GlobalVariables::SetValue(const std::string &groupName, const ITEM &item) {
	SetValue(groupName, item.GetKey(), item.GetItem());
}
