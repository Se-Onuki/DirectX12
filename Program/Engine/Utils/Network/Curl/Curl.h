/// @file Curl.h
/// @brief Curlのヘッダ
/// @author ONUKI seiya
#pragma once

#include "../WindowsSocket/WindowsSocket.h"	//	WindowsSocketのヘッダ
#include "../../Success.h"					//	成否表現
#include <curl/curl.h>						//	Curlのヘッダ

namespace SoLib {

	namespace Network {
		/// @brief CURLの書き込みコールバック関数
		/// @param ptr[in] 書き込むデータ
		/// @param size[in] データのサイズ
		/// @param nmemb[in] データの個数
		/// @param userdata[in] ユーザーデータ
		/// @return 書き込んだバイト数
		size_t WriteCallback(char *const ptr, const size_t size, const size_t nmemb, std::string *const userdata);
	}

	class Curl {
	public:

		struct Status {
			enum Id :uint32_t {
				kSuccess,			// 成功
				kInitError,			// 初期化エラー
				kSetOptionError,	// オプション設定エラー
				kPerformError,		// 実行エラー
				kUnknownError,		// 不明なエラー
			} id_ = kUnknownError;

			Status() = default;
			Status(const Id id) noexcept : id_(id) {}

			inline operator Id() const noexcept {
				return id_;
			}
			inline explicit operator bool() const noexcept {
				// 成功ならばtrueを返す
				return id_ == kSuccess;
			}
			inline explicit operator const std::string &() const noexcept {
				return StatusToString(*this);
			}

			/// @brief ステータスから文字列に変換する
			/// @param status[in] Curlステータス 
			/// @return ステータスの文字列
			static const std::string StatusToString(const Status &status);
		};

	public:
		Curl() = default;
		~Curl() {
			// クリーンアップ処理を行う
			CleanUp();
		}
		Curl(const Curl &) = delete;
		Curl &operator=(const Curl &) = delete;
		Curl(Curl &&other) noexcept : curl_(other.curl_) {
			other.curl_ = nullptr;
		}
		Curl &operator=(Curl &&other) noexcept {
			// もし自分自身と同じならば何もしない
			if (this != &other) {
				// 管理するアドレスを移譲する
				curl_ = other.curl_;
				other.curl_ = nullptr;
			}
			return *this;
		}
		/// @brief クリーンアップ処理
		/// @return クリーンアップに成功したかどうか
		IsSuccess CleanUp();

		Status Init();

		template<typename... Args>
		CURLcode SetOption(const CURLoption option, Args... args) {
			// curlのオプションを設定する
			return curl_easy_setopt(curl_, option, args...);
		}

		CURLcode Perform() {
			// curlの実行
			return curl_easy_perform(curl_);
		}

		template<typename... Args>
		CURLcode GetInfo(const CURLINFO info, Args... args) const {
			return curl_easy_getinfo(curl_, info, args...);
		}

		inline explicit operator bool() const noexcept {
			// curlのポインタが有効ならばtrueを返す
			return curl_ != nullptr;
		}



	private:

		// curlのポインタ
		CURL *curl_ = nullptr;

	};

}