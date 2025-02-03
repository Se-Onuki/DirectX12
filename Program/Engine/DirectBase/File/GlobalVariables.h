/// @file GlobalVariables.h
/// @brief Jsonで保存するグローバル変数
/// @author ONUKI seiya
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
#include "../../Utils/Math/Math.hpp"

class GlobalVariables {
	GlobalVariables() = default;
	GlobalVariables(const GlobalVariables &) = delete;
	GlobalVariables operator=(const GlobalVariables &) = delete;
	~GlobalVariables() = default;

	/// @brief Jsonファイルの保存場所
	const std::string kDirectoryPath = "resources/GlobalVariables/";

public:
	/// @brief 保存できる型
	using Item = std::variant<int32_t, float, Vector2, Vector3, Vector4, SoLib::Angle::Radian, std::string>;
	/// @brief 1つのグループに保存する連想記憶コンテナ
	using Group = std::unordered_map<std::string, Item>;

	static GlobalVariables *const GetInstance() {
		static GlobalVariables instance{};
		return &instance;
	}

	/// @brief グループを作る
	/// @param[in] groupName グループ名
	void CreateGroups(const std::string &groupName) { datas_[groupName]; }

	/// @brief Group自体のゲッタ
	/// @param[in] groupName グループ名
	/// @return 紐づいた Group
	const Group &GetGroup(const std::string &groupName) const;
	Group &GetGroup(const std::string &groupName);

	Group &operator[](const std::string &groupName) { return datas_[groupName]; }

	/// @brief Item自体のゲッタ
	/// @param[in] groupName グループ名
	/// @param[in] key 紐づけられたキー
	/// @return 紐づいた Item
	const Item &Get(const std::string &groupName, const std::string &key) const;

	/// @brief 変換付きゲッタ
	/// @tparam T 変換先の型
	/// @param[in] groupName グループ名
	/// @param[in] key 紐づけられたキー
	/// @return 型変換した値
	template<typename T>
	T Get(const std::string &groupName, const std::string &key) const;

	/// @brief 値の追加
	/// @tparam T 追加する型
	/// @param[in] groupName グループ名
	/// @param[in] key 変数名
	/// @param[in] value 保存する値
	template<typename T>
	void AddValue(const std::string &groupName, const std::string &key, const T &value);

	/// @brief 値の更新
	/// @tparam T 更新する型
	/// @param[in] groupName グループ名
	/// @param[in] key 変数名
	/// @param[in] value 更新する値
	template<typename T>
	void SetValue(const std::string &groupName, const std::string &key, const T &value);

	/// @brief 値の追加
	/// @tparam ITEM 追加する型と名前のペア
	/// @param[in] groupName グループ名
	/// @param[in] item 追加する値と名前
	template <typename ITEM, SoLib::Text::ConstExprString V = ITEM::str_, SoLib::IsRealType T = typename ITEM::ItemType>
		requires(std::same_as<ITEM, VariantItem<V, T>>)
	void AddValue(const std::string &groupName, const ITEM &item);

	/// @brief 値の更新
	/// @tparam ITEM 更新する型と名前のペア
	/// @param[in] groupName グループ名
	/// @param[in] item 更新する値と名前
	template <typename ITEM, SoLib::Text::ConstExprString V = ITEM::str_, SoLib::IsRealType T = typename ITEM::ItemType>
		requires(std::same_as<ITEM, VariantItem<V, T>>)
	void SetValue(const std::string &groupName, const ITEM &item);

	/// @brief 更新
	void Update();

	/// @brief Jsonファイルを保存
	/// @param groupName 保存するグループ名
	void SaveFile(const std::string &groupName) const;

	/// @brief Jsonファイルを読み込む
	void LoadFile();
	/// @param[in] groupName グループ名
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
/// @param item 保存されたItem
/// @param value 代入先の変数
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

/// @brief Itemから型変換を行って代入
/// @tparam T 代入する型
/// @param group 保存されたグループ
/// @param value 代入先の変数
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

/// @brief Itemから型変換を行ってJson出力
/// @tparam T 代入する型
/// @param group 保存先のグループ
/// @param value 代入する変数
template <typename ITEM, SoLib::Text::ConstExprString V = ITEM::str_, SoLib::IsRealType T = typename ITEM::ItemType>
	requires(std::same_as<ITEM, VariantItem<V, T>>)
void operator<< (GlobalVariables::Group &group, const ITEM &value) {
	group[value.GetKey()] = value.GetItem();
}

/// @brief 値の取得
/// @tparam T 取得する型
/// @param[in] groupName グループ名
/// @param[in] key キー
/// @return 型変換した値
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

/// @brief 値の追加
/// @tparam T 追加する型
/// @param[in] groupName グループ名
/// @param[in] key キー
/// @param[in] value 保存する値
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

/// @brief 値の更新
/// @tparam T 更新する型
/// @param[in] groupName グループ名
/// @param[in] key キー
/// @param[in] value 更新する値
template<typename T>
void GlobalVariables::SetValue(
	const std::string &groupName, const std::string &key, const T &value) {
	Group &group = datas_[groupName];
	group[key] = value;
}

/// @brief 値の追加
/// @param[in] groupName グループ名
/// @param[in] item 追加するアイテム
template <typename ITEM, SoLib::Text::ConstExprString V,  SoLib::IsRealType T>
	requires(std::same_as<ITEM, VariantItem<V, T>>)
inline void GlobalVariables::AddValue(const std::string &groupName, const ITEM &item) {
	AddValue(groupName, item.GetKey(), item.GetItem());
}

/// @brief 値の更新
/// @param[in] groupName グループ名
/// @param[in] item 更新するアイテム
template <typename ITEM, SoLib::Text::ConstExprString V, SoLib::IsRealType T>
	requires(std::same_as<ITEM, VariantItem<V, T>>)
inline void GlobalVariables::SetValue(const std::string &groupName, const ITEM &item) {
	SetValue(groupName, item.GetKey(), item.GetItem());
}
