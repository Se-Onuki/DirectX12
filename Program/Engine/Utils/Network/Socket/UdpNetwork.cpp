#include "UdpNetwork.h"
namespace SoLib {

	std::unique_ptr<UdpNetwork> UdpNetwork::GenerateServer(std::optional<uint16_t> port)
	{
		std::unique_ptr<Network> network = std::make_unique<Network>();
		network->SetSocketType(Network::SocketType::kUDP);
		if (port) { network->SetPortID(*port); }
		if (not network->Init()) {
			return nullptr;
		}

		std::unique_ptr<UdpNetwork> result = std::make_unique<UdpNetwork>(std::move(network));

		// 名前付け
		if (not result->Bind(result->network_->GetPortID(), INADDR_ANY)) {
			return nullptr;
		}

		return std::move(result);
	}

	std::unique_ptr<UdpNetwork> UdpNetwork::GenerateClient(uint16_t port, const std::string_view &ip)
	{
		std::unique_ptr<Network> network = std::make_unique<Network>();
		network->SetSocketType(Network::SocketType::kUDP);
		if (not network->Init()) {
			return nullptr;
		}
		std::unique_ptr<UdpNetwork> result = std::make_unique<UdpNetwork>(std::move(network));

		// 
		if (not result->SetSockAddr(port, ip)) {
			return nullptr;
		}

		return std::move(result);
	}

	IsSuccess UdpNetwork::Bind(const uint16_t port, const uint32_t addr)
	{
		SOCKADDR_IN saddr;
		// メモリの初期化
		std::memset(&saddr, 0, sizeof(SOCKADDR_IN));
		saddr.sin_family = AF_INET;		// アドレスファミリ
		saddr.sin_port = port;			// ポート番号
		saddr.sin_addr.s_addr = addr;	// すべてのIPアドレスを許容する

		// バインドを実行｡
		int32_t errorCheck = bind(static_cast<SOCKET>(network_->GetSocket()), std::bit_cast<const SOCKADDR *>(&saddr), sizeof(SOCKADDR));
		return errorCheck != SOCKET_ERROR;
	}

	IsSuccess UdpNetwork::SetSockAddr(const uint16_t port, const std::string_view &ip)
	{
		auto ipAddr = inet_addr(ip.data());

		client_.sin_family = AF_INET;
		client_.sin_port = port;
		client_.sin_addr.s_addr = ipAddr;
		// Ip指定が成功したらtrue
		return ipAddr != INADDR_NONE;
	}

	int32_t UdpNetwork::Send(const void *source, int32_t byte) const
	{
		return network_->GetSocket().SendTo(source, byte, client_);
	}

	int32_t UdpNetwork::Recv(void *rec, int32_t byte)
	{
		return network_->GetSocket().RecvFrom(rec, byte, client_);
	}
}