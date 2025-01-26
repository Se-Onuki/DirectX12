#pragma once
#include "Socket/OwnedSocket.h"

class Network
{
public:
	enum class SocketType : uint8_t {
		kTCP = SOCK_STREAM,	// TCP
		kUDP = SOCK_DGRAM,	// UDP
	};

	Network() = default;
	~Network() = default;

	/// @brief ポート設定
	/// @param socket ポート番号
	void SetPortID(uint16_t port) { port_ = port; }
	/// @brief ポート番号の取得
	/// @return ポート番号
	uint16_t GetPortID() const { return port_; }

	/// @brief ソケットのタイプを指定する
	/// @param socketType ソケットタイプ
	void SetSocketType(SocketType socketType) { socketType_ = socketType; }

	/// @brief ソケットタイプを返す
	/// @return ソケットタイプ
	SocketType GetSocketType() const { return socketType_; }

	/// @brief ソケットの作成
	/// @return 成功したなら真を返す
	IsSuccess Init();

	/// @brief ソケット識別子の取得
	/// @return ソケット識別子
	OwnedSocket &GetSocket() { return socketID_; }

private:
	// ソケット識別子
	OwnedSocket socketID_;

	// 通信タイプ
	SocketType socketType_ = SocketType::kTCP;
	// ネットワークのポート番号
	uint16_t port_ = 8000u;

};

inline IsSuccess Network::Init()
{
	// ソケットの作成
	socketID_ = OwnedSocket::Generate(AF_INET, static_cast<int32_t>(socketType_), 0);
	return static_cast<bool>(socketID_);
}
