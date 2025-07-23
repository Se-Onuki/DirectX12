/// @file String.h
/// @brief 文字列の変換
/// @author ONUKI seiya
#pragma once
#include <Windows.h>
#include <string>


namespace SolEngine {
	/// @brief ワイド文字列と文字列の相互変換
	/// @param[in] str 変換する文字列
	/// @return 変換後の文字列
	inline std::wstring ConvertString(const std::string &str) {
		if (str.empty()) {
			return std::wstring();
		}
		auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char *>(&str[0]), static_cast<int>(str.size()), NULL, 0);
		if (sizeNeeded == 0) {
			return std::wstring();
		}
		std::wstring result(sizeNeeded, 0);
		MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char *>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
		return result;
	}

	inline std::string ConvertString(const std::wstring &str) {
		if (str.empty()) {
			return std::string();
		}
		auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
		if (sizeNeeded == 0) {
			return std::string();
		}
		std::string result(sizeNeeded, 0);
		WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
		return result;
	}
}