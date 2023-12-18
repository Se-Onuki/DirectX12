#pragma once
#include <cstdint>
#include <array>
#include <string>
#include "SoLib/SoLib_ImGui.h"
#include "SoLib/SoLib_Json.h"

template<uint32_t size>
class ConstString {
public:
	using StringType = std::array<char, size>;
	ConstString(const char *const name = "") {
		*this = name;
	}

	/// @brief 文字列の代入
	/// @param str 文字列
	/// @return 文字列クラス
	inline ConstString &operator=(const char *const str) {
		std::memcpy(string_.data(), str, (std::min<uint32_t>)(size - 1u, static_cast<uint32_t>(std::strlen(str) + 1u)));
		string_[size - 1u] = '\0';
		return *this;
	}

	/// @brief 文字列の代入
	/// @param str 文字列
	/// @return 文字列クラス
	inline ConstString &operator=(const std::string &str) {
		std::memcpy(string_.data(), str.data(), (std::min<uint32_t>)(size - 1u, static_cast<uint32_t>(str.size() + 1u)));

		return *this;
	}

	inline bool operator==(const ConstString &str) const {
		return not(std::strcmp(this->data(), str.data()));
	}

	inline bool operator==(const char *const str) const {
		return not(std::strcmp(this->data(), str));
	}

	bool ImGuiWidget(const char *const label);

	operator char *const () { return string_.data(); }
	operator const char *const () const { return string_.data(); }

	char *const data() { return string_.data(); }
	const char *const data()const { return string_.data(); }

	StringType::iterator begin() { return string_.begin(); }
	StringType::const_iterator begin() const { return string_.begin(); }

	StringType::iterator end() { return string_.end(); }
	StringType::const_iterator end() const { return string_.end(); }

private:
	// 文字列
	std::array<char, size> string_;
};

template<uint32_t size>
inline bool ConstString<size>::ImGuiWidget(const char *const label) {
#ifdef _DEBUG

	return ImGui::InputText(label, this->data(), size);

#else
	label;
	return false;

#endif // _DEBUG

}

template<uint32_t size>
void to_json(nlohmann::json &json, const ConstString<size> &value) {
	json = value.data();
}

template<uint32_t size>
void from_json(const nlohmann::json &json, ConstString<size> &value) {
	value = json.get<std::string>();
}
