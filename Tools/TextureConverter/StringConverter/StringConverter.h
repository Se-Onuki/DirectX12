#pragma once
#include <Windows.h>
#include <string>

namespace SoLib {
	/// @brief stringからwstringへの変換
	/// @param str char_t型文字列
	/// @return wchar_t型文字列
	inline std::wstring ConvertString(const std::string_view &str) {
		if (str.empty()) {
			return std::wstring();
		}
		auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0);
		if (sizeNeeded == 0) {
			return std::wstring();
		}
		std::wstring result(sizeNeeded, 0);
		MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), &result[0], sizeNeeded);
		return result;
	}

	/// @brief wstringからstringへの変換
	/// @param str wchar_t型文字列
	/// @return char_t型文字列
	inline std::string ConvertString(const std::wstring_view &str) {
		if (str.empty()) {
			return std::string();
		}
		auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int32_t>(str.size()), NULL, 0, NULL, NULL);
		if (sizeNeeded == 0) {
			return std::string();
		}
		std::string result(sizeNeeded, 0);
		WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int32_t>(str.size()), result.data(), sizeNeeded, NULL, NULL);
		return result;
	}
}