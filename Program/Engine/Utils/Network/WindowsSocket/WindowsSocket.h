#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS	// おそらくエラー対応 _?_
#include <winsock2.h>					// ネットワーク関係
#pragma comment(lib, "WSock32.lib")		// ネットワークのライブラリの適用

#include <cstdint>
#include <optional>
#include "../../Success.h"

namespace SoLib {

	class WindowsSocket {
	public:
		WindowsSocket() = default;

		~WindowsSocket() { Finalize(); }

		/// @brief 初期化処理
		/// @param a 
		/// @param b 
		/// @return 初期化の成否
		IsSuccess Init(uint8_t a, uint8_t b);

		IsSuccess Finalize();

	private:

		std::optional<WSADATA> wsaData_ = std::nullopt;
	};
}