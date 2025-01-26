#include "WindowsSocket.h"

IsSuccess WindowsSocket::Init(uint8_t a, uint8_t b)
{
	// データを持っていたら終わる
	if (wsaData_) {
		return false;
	}

	// 持ってなかったら
	// データを構築する
	return WSAStartup(MAKEWORD(a, b), &wsaData_.emplace()) == 0; // 構築に成功したならtrue

	// _!_ std::optional::emplace(Args...) -> T&
	// コンストラクタ(Args...)->構築後アドレス
	// https://cpprefjp.github.io/reference/optional/optional/emplace.html
}

IsSuccess WindowsSocket::Finalize()
{
	// データを持っていなかったら
	if (not wsaData_) {
		// すでに終わったものとする
		return false;
	}
	// ネットワークから離脱
	WSACleanup();
	// データをリセット
	wsaData_ = std::nullopt;
	return true;

}