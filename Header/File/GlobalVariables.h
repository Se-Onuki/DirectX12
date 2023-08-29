#pragma once
#include "../Math/Vector2.h"
#include "../Math/Vector3.h"
#include "../Math/Vector4.h"
#include <assert.h>
#include <stdint.h>
#include <string>
#include <unordered_map>
#include <variant>

class GlobalVariables {
	GlobalVariables() = default;
	GlobalVariables(const GlobalVariables &) = delete;
	GlobalVariables operator=(const GlobalVariables &) = delete;
	~GlobalVariables() = default;

	const std::string kDirectoryPath = "Resources/GlobalVariables/";

public:
	/// @brief 保存できる型
	using Item = std::variant<int32_t, float, Vector2, Vector3, Vector4>;
	/// @brief 1つのグループに保存する連想記憶コンテナ
	using Group = std::unordered_map<std::string, Item>;

	static GlobalVariables *const GetInstance() {
		static GlobalVariables instance{};
		return &instance;
	}

	void CreateGroups(const std::string &groupName) { datas_[groupName]; }

	/// @brief Item自体のゲッタ
	/// @param groupName グループ名
	/// @param key 紐づけられたキー
	/// @return 紐づけられたItem
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
void operator<< (T &value, const GlobalVariables::Item &item) {

	// 型が正しいか
	if (std::holds_alternative<T>(item)) {
		value = std::get<T>(item);
		return;
	}
	assert(0 && "存在しない型を取得しようとしました。");
	value = T{};
}

template<typename T>
inline T GlobalVariables::Get(const std::string &groupName, const std::string &key) const {
	// グループ内を検索
	auto itGroup = datas_.find(groupName);
	assert(itGroup != datas_.end());

	// キーがあるか
	auto itItem = itGroup->second.find(key);
	assert(itItem != itGroup->second.end());

	// 型が正しいか
	if (std::holds_alternative<T>(itItem->second)) {
		return std::get<T>(itItem->second);
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

template <typename T>
class VariantItem final {
	VariantItem() = delete;
public:
	VariantItem(const std::string &key, const T &item = {}) : key_(key), item_(item) {}
	inline VariantItem &operator=(const T &item);
	~VariantItem() = default;

	inline operator T();
	inline operator T() const;

private:
	const std::string key_;
	T item_;
};

template<typename T>
inline VariantItem<T> &VariantItem<T>::operator=(const T &item) {
	item_ = item;
	return *this;
}

template<typename T>
inline VariantItem<T>::operator T() {
	return item_;
}

template<typename T>
inline VariantItem<T>::operator T() const {
	return item_;
}
