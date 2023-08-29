#pragma once

#include "../../DirectBase/Base/SoLib.h"

#include <string>

template <typename T>
class VariantItem final {
	VariantItem() = delete;
public:
	VariantItem(const std::string &key, const T &item = {}) : key_(key), item_(item) {}
	inline VariantItem &operator=(const T &item);
	~VariantItem() = default;

	//inline operator T();

	inline operator const T &() const;

	const std::string &GetKey() const { return key_; }
	const T &GetItem() const { return item_; }

	void ImGuiWidget() { SoLib::ImGuiWidget(key_.c_str(), &item_); }

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
inline VariantItem<T>::operator const T &() const {
	return item_;
}