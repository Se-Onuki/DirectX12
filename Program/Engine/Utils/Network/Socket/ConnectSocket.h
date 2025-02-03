#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS	// おそらくエラー対応 _?_
#include <winsock2.h>					// ネットワーク関係
#pragma comment(lib, "WSock32.lib")		// ネットワークのライブラリの適用

#include <iterator>	// イテレータなど
#include <cstdint>	// 規定型のエイリアス
#include <cassert>	// エラー停止
#include <bit>		// bitに関する処理
namespace SoLib {

	/// @brief 受送信を行うソケット
	class ConnectSocket {
	public:
		ConnectSocket() = default;
		ConnectSocket(const SOCKET &sock) :socket_(sock) {}

		/// @brief 受信処理
		/// @param begin[out] 始点
		/// @param end[out] 番兵
		/// @return 受信メモリ数
		template<std::contiguous_iterator T>
		int32_t Recv(T begin, T end);

		/// @brief 受信処理
		/// @param[out] recvPtr 
		/// @param[in] byte 
		/// @return 受信メモリ数
		int32_t Recv(void *recvPtr, int32_t byte) const;


		template<std::contiguous_iterator T>
		int32_t RecvFrom(T begin, T end, SOCKADDR_IN &sockaddr) const;
		int32_t RecvFrom(void *recvPtr, int32_t byte, SOCKADDR_IN &sockaddr) const;

		template<std::contiguous_iterator T>
		int32_t Send(T begin, T end);
		int32_t Send(const void *source, int32_t byte) const;


		template<std::contiguous_iterator T>
		int32_t SendTo(T begin, T end, const SOCKADDR_IN &sockaddr) const;
		int32_t SendTo(const void *source, int32_t byte, const SOCKADDR_IN &sockaddr) const;


		/// @brief 有効なデータを持っているかどうか
		explicit operator bool() const { return IsActive(); }

		/// @brief 有効なデータを持っているかどうか
		bool IsActive() const {
			return not (socket_ == static_cast<SOCKET>(SOCKET_ERROR) or socket_ == static_cast<SOCKET>(INVALID_SOCKET));
		}

		/// @brief SOCKETへの変換
		explicit operator const SOCKET() const { return socket_; }
		/// @brief SOCKETへの変換
		explicit operator SOCKET &() { return socket_; }

	protected:
		// ソケット
		SOCKET socket_ = static_cast<SOCKET>(SOCKET_ERROR);
	};


	template<std::contiguous_iterator T>
	inline int32_t ConnectSocket::Recv(T begin, T end)
	{
		size_t distance = std::distance(begin, end);
		return Recv((&*begin), static_cast<int32_t>(distance));
	}

	inline int32_t ConnectSocket::Recv(void *recvPtr, int32_t byte) const
	{
		assert(static_cast<bool>(*this) and "ConnectSocket : 接続先がありません｡");
		return recv(socket_, std::bit_cast<char *>(recvPtr), byte, 0);
	}

	inline int32_t ConnectSocket::RecvFrom(void *recvPtr, int32_t byte, SOCKADDR_IN &sockaddr) const
	{
		int32_t fromLen{};
		return recvfrom(socket_, std::bit_cast<char *>(recvPtr), byte, 0, std::bit_cast<SOCKADDR *>(&sockaddr), &fromLen);
	}

	template<std::contiguous_iterator T>
	inline int32_t ConnectSocket::Send(T begin, T end)
	{
		size_t distance = std::distance(begin, end);
		return Send((&*begin), static_cast<int32_t>(distance));
	}

	inline int32_t ConnectSocket::Send(const void *source, int32_t byte) const
	{
		assert(static_cast<bool>(*this) and "ConnectSocket : 接続先がありません｡");

		return send(socket_, std::bit_cast<char *>(source), byte, 0);
	}

	template<std::contiguous_iterator T>
	inline int32_t ConnectSocket::SendTo(T begin, T end, const SOCKADDR_IN &sockaddr) const
	{
		size_t distance = std::distance(begin, end);
		return SendTo((&*begin), static_cast<int32_t>(distance), sockaddr);
	}

	inline int32_t ConnectSocket::SendTo(const void *source, int32_t byte, const SOCKADDR_IN &sockaddr) const
	{
		return sendto(socket_, std::bit_cast<const char *>(source), byte, 0, std::bit_cast<const SOCKADDR *>(&sockaddr), sizeof(SOCKADDR));
	}

}