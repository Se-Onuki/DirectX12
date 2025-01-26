#pragma once
#include <memory>
#include "../Network.h"
#include <optional>

class UdpNetwork
{
public:
	UdpNetwork() = default;
	UdpNetwork(std::unique_ptr<Network> &&network) : network_(std::move(network)) {}
	~UdpNetwork() = default;


	static std::unique_ptr<UdpNetwork> GenerateServer(std::optional<uint16_t> port);

	static std::unique_ptr<UdpNetwork> GenerateClient(uint16_t port, const std::string_view &ip);


	int32_t Send(const void *source, int32_t byte) const;

	int32_t Recv(void *rec, int32_t byte);

private:

	/// @brief 名前の設定
	/// @param port 
	/// @param addr 
	/// @return バインドの成否
	IsSuccess Bind(const uint16_t port, const uint32_t addr = INADDR_ANY);

	/// @brief 接続先の指定
	/// @param port 接続先のポート
	/// @param ip 接続先のIP
	/// @return 設定が完了したか
	IsSuccess SetSockAddr(const uint16_t port, const std::string_view &ip);


private:
	// ネットワーク
	std::unique_ptr<Network> network_;

	SOCKADDR_IN client_{};

};