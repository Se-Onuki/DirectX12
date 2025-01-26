#pragma once
#include "../../Success.h"
#include "ConnectSocket.h"

class OwnedSocket : public ConnectSocket
{
public:
	OwnedSocket() = default;
	OwnedSocket(const OwnedSocket &) = delete;
	OwnedSocket(SOCKET &&sock) {
		socket_ = sock;
		sock = static_cast<SOCKET>(SOCKET_ERROR);
	}
	OwnedSocket(OwnedSocket &&that) noexcept {
		socket_ = that.socket_;
		that.socket_ = static_cast<SOCKET>(SOCKET_ERROR);
	}
	OwnedSocket &operator=(const OwnedSocket &) = delete;
	OwnedSocket &operator=(SOCKET &&sock) {
		socket_ = sock;
		sock = static_cast<SOCKET>(SOCKET_ERROR);
		return *this;
	}
	OwnedSocket &operator=(OwnedSocket &&that) noexcept {
		socket_ = that.socket_;
		that.socket_ = static_cast<SOCKET>(SOCKET_ERROR);
		return *this;
	}
	~OwnedSocket() {
		Finalize();
	}

	static OwnedSocket Generate(int32_t af, int32_t type, int32_t protocol) {
		return OwnedSocket{ socket(af,type,protocol) };
	}

	IsSuccess Bind(const uint16_t port, const uint32_t addr = INADDR_ANY) {
		SOCKADDR_IN saddr;
		// メモリの初期化
		std::memset(&saddr, 0, sizeof(SOCKADDR_IN));
		saddr.sin_family = AF_INET;		// アドレスファミリ
		saddr.sin_port = port;			// ポート番号
		saddr.sin_addr.s_addr = addr;	// すべてのIPアドレスを許容する

		// バインドを実行｡
		int32_t errorCheck = bind(socket_, std::bit_cast<const SOCKADDR *>(&saddr), sizeof(SOCKADDR));
		return errorCheck != static_cast<SOCKET>(SOCKET_ERROR);
	}

	/// @brief 終了処理
	/// @return 終了成功したらTrue
	IsSuccess Finalize();
private:

};

inline IsSuccess OwnedSocket::Finalize() {
	if (IsActive()) {
		closesocket(socket_);
		socket_ = static_cast<SOCKET>(SOCKET_ERROR);
		return true;
	}
	return false;
}