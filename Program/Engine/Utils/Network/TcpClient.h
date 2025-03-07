#pragma once
#include <memory>
#include "Network.h"
namespace SoLib {

	class TcpClient {
	public:
		TcpClient() = default;
		TcpClient(std::unique_ptr<Network> &&network) :network_(std::move(network)) {}
		~TcpClient() = default;
	public:

		static std::unique_ptr<TcpClient> Generate(const char *hostIp, uint16_t port);

		IsSuccess Init(const char *hostIp, uint16_t port);

		/// @brief ホストの情報の検索
		/// @param hostIp ホストのIP
		/// @return ホストの情報へのアドレス
		LPHOSTENT SetHost(const char *hostIp);

		/// @brief ホストへの接続
		/// @param port ホストへのポート
		/// @return 
		IsSuccess Connect(uint16_t port);

		ConnectSocket &GetSocket() { return network_->GetSocket(); }

	private:

		// ホストサーバのデータ
		LPHOSTENT lpHost_ = nullptr;

		// ネットワーク
		std::unique_ptr<Network> network_;

	};

	inline std::unique_ptr<TcpClient> TcpClient::Generate(const char *hostIp, uint16_t port)
	{
		std::unique_ptr<Network> network = std::make_unique<Network>();
		network->SetPortID(port);
		if (not network->Init()) {
			return nullptr;
		}
		auto client = std::make_unique<TcpClient>(std::move(network));
		if (not client->Init(hostIp, port)) {
			return nullptr;
		}
		return std::move(client);
	}

	inline IsSuccess TcpClient::Init(const char *hostIp, uint16_t port)
	{
		// ホストの検索
		auto ptr = SetHost(hostIp);
		// もしホストがエラーなら失敗を返す
		if (ptr == nullptr) { return false; }

		// 接続を行う
		return Connect(port);
	}

	inline LPHOSTENT TcpClient::SetHost(const char *hostIp)
	{
		// ホストのIPからホストの情報を取得する
		lpHost_ = gethostbyname(hostIp);
		return lpHost_;
	}

	inline IsSuccess TcpClient::Connect(uint16_t port)
	{
		SOCKADDR_IN sAddr;
		std::memset(&sAddr, 0, sizeof(SOCKADDR_IN));
		sAddr.sin_family = lpHost_->h_addrtype;
		sAddr.sin_port = port;
		sAddr.sin_addr.s_addr = *((uint32_t *)lpHost_->h_addr);

		// サーバのポートを保存する
		network_->SetPortID(port);
		// 接続してエラーでなければSuccess
		return connect(static_cast<SOCKET>(network_->GetSocket()), std::bit_cast<SOCKADDR *>(&sAddr), sizeof(SOCKADDR_IN)) != SOCKET_ERROR;
	}

}