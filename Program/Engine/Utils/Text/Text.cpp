#include "Text.h"
#include <Windows.h>
#include "../../../Engine/DirectBase/Base/WinApp.h" 

namespace SoLib::Text {

	IsSuccess CopyClipboard(const std::string_view &text) noexcept
	{
		// ワイド文字列に変換して､クリップボードにコピーする
		return CopyClipboard(ConvertString(text));
	}

	IsSuccess CopyClipboard(const std::wstring_view &text) noexcept
	{
		auto hwnd = WinApp::GetInstance()->GetHWND();

		// 文字列の長さ
		auto len = (text.size()) * 2; // ワイド文字列は1文字あたり2バイトなので、長さを2倍にする
		HGLOBAL hMem = GlobalAlloc(GHND, len);
		if (hMem == nullptr) {
			return false; // メモリの確保に失敗
		}

		const auto pMem = reinterpret_cast<wchar_t *>(GlobalLock(hMem));
		if (pMem == nullptr) {
			GlobalFree(hMem);
			return false; // メモリのロックに失敗
		}

		std::copy(text.begin(), text.end(), pMem);
		GlobalUnlock(hMem);
		OpenClipboard(hwnd);
		EmptyClipboard();

		if (SetClipboardData(CF_UNICODETEXT, hMem) == nullptr) {
			GlobalFree(hMem);
			CloseClipboard();
			return false; // クリップボードへのデータ設定に失敗
		}

		CloseClipboard();

		GlobalFree(hMem);

		return true;
	}

	std::wstring ConvertString(const std::string_view &str) {
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

	std::string ConvertString(const std::wstring_view &str) {
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