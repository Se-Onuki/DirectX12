#pragma once
#include <memory>
#include <vector>
#include <span>

#include "Network.h"
namespace SoLib {

	class TcpServer {
	public:
		TcpServer() = default;
		TcpServer(std::unique_ptr<Network> &&network) :network_(std::move(network)) {}
		~TcpServer() = default;

		static std::unique_ptr<TcpServer> Generate(const uint16_t port);

		IsSuccess Init();

		/// @brief 名前の設定
		/// @param port ポート番号
		/// @param addr アドレス開放設定
		/// @return 命名に成功したか
		IsSuccess Bind(const uint16_t port, const uint32_t addr = INADDR_ANY);

		/// @brief 接続処理と､待機を行う
		/// @param backLog 
		/// @return 接続が成功したか
		IsSuccess ConnectionWait(int32_t backLog = 0);

		/// @brief 接続開始
		/// @param backLog 
		/// @return 処理が成功したか
		IsSuccess Listen(int32_t backLog);

		/// @brief 接続待機
		/// @return 接続先のデータ{ 接続先のクライアント, 接続先の情報 }
		std::pair<ConnectSocket, SOCKADDR_IN> Accept();

		ConnectSocket &GetClient(size_t i = 0) { return client_[i]; }


	private:
		// クライアントの情報
		std::vector<ConnectSocket> client_;

		// ネットワーク
		std::unique_ptr<Network> network_;

	};



	inline std::unique_ptr<TcpServer> TcpServer::Generate(const uint16_t port)
	{
		std::unique_ptr<Network> network = std::make_unique<Network>();
		network->SetPortID(port);
		if (not network->Init()) {
			return nullptr;
		}
		// サーバの生成
		std::unique_ptr<TcpServer> result = std::make_unique<TcpServer>(std::move(network));

		// バインドの実行
		if (not result->Bind(port)) {
			return nullptr;
		}

		return std::move(result);
	}

	inline IsSuccess TcpServer::Init()
	{

		// バインドを実行
		if (not Bind(network_->GetPortID(), INADDR_ANY)) {
			return IsSuccess{ false };	// エラーを返す
		}

		// 接続開始
		if (not Listen(0)) {
			return IsSuccess{ false };	// エラーを返す
		}

		// 接続待機
		auto client = Accept();

		// 接続成功したら
		if (client.first) {
			// 接続先を保存
			client_.push_back(client.first);
		}

		return IsSuccess{ client.first.IsActive() }; // 成否判定を返す

	}

	//
	inline IsSuccess TcpServer::Bind(const uint16_t port, const uint32_t addr)
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

	inline IsSuccess TcpServer::ConnectionWait(int32_t backLog)
	{
		// 接続開始
		if (not Listen(backLog)) {
			return IsSuccess{ false };	// エラーを返す
		}

		// 接続待機
		auto client = Accept();

		// 接続成功したら
		if (client.first) {
			// 接続先を保存
			client_.push_back(client.first);
		}
		return IsSuccess{ client.first.IsActive() };
	}

	inline IsSuccess TcpServer::Listen(int32_t backLog)
	{
		return listen(static_cast<SOCKET>(network_->GetSocket()), backLog) != SOCKET_ERROR;
	}

	inline std::pair<ConnectSocket, SOCKADDR_IN> TcpServer::Accept()
	{
		std::pair<ConnectSocket, SOCKADDR_IN> result;

		// データのサイズ
		int32_t targetSize = sizeof(SOCKADDR_IN);

		// 接続まで待機
		result.first = accept(static_cast<SOCKET>(network_->GetSocket()), std::bit_cast<SOCKADDR *>(&result.second), &targetSize);
		if (not result.first) {	// エラーソケット検知
			return {};	// エラーを返す
		}

		return result;
	}
}