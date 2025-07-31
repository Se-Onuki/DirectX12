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
		// ウィンドウのハンドルを取得する
		auto hwnd = WinApp::GetInstance()->GetHWND();

		// 文字列の長さ
		auto len = text.size() * 2; // ワイド文字列は1文字あたり2バイトなので、長さを2倍にする
		// 文字列を保存するためのメモリを確保
		HGLOBAL hMem = GlobalAlloc(GHND, len);
		// メモリの確保に失敗した場合
		if (hMem == nullptr) {
			return false; // メモリの確保に失敗
		}

		// メモリをロックして､書き込み先のポインタを取得
		const auto pMem = reinterpret_cast<wchar_t *>(GlobalLock(hMem));
		// メモリのロックに失敗した場合
		if (pMem == nullptr) {
			// メモリを開放する
			GlobalFree(hMem);
			return false; // メモリのロックに失敗
		}

		// 文字列をコピペ
		std::copy(text.begin(), text.end(), pMem);
		// ロックを解除する
		GlobalUnlock(hMem);
		// クリップボード開く
		OpenClipboard(hwnd);
		// クリップボードを空にする
		EmptyClipboard();

		// クリップボードにデータを設定
		if (SetClipboardData(CF_UNICODETEXT, hMem) == nullptr) {

			// メモリを開放して､クリップボードを閉じる
			GlobalFree(hMem);
			// クリップボードを閉じる
			CloseClipboard();
			return false; // クリップボードへのデータ設定に失敗
		}

		// メモリを開放して､クリップボードを閉じる
		GlobalFree(hMem);
		CloseClipboard();

		return true;
	}

	std::wstring ConvertString(const std::string_view &str) {
		// もし仮に文字列が空だった場合は空文字を返す
		if (str.empty()) {
			return std::wstring();
		}
		// 変換後の文字数を取得する
		auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char *>(&str[0]), static_cast<int>(str.size()), NULL, 0);
		// もし変換後の文字数が0だった場合は空文字を返す
		if (sizeNeeded == 0) {
			return std::wstring();
		}
		// 文字数分のメモリを確保する
		std::wstring result(sizeNeeded, 0);
		// 文字列を変換して格納する
		MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char *>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
		// 変換後の文字列を返却する
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