#include "Curl.h"

namespace SoLib {
	size_t Network::WriteCallback(char *const ptr, const size_t size, const size_t nmemb, std::string *const userdata)
	{
		// もし正しいポインタが渡されていない場合は0を返す
		if (userdata == nullptr or ptr == nullptr) {
			return 0;
		}

		// 文字列の末端に､アドレスptrを始点としたsize*nmembバイトの文字列を追加する
		userdata->append(ptr, size * nmemb);

		return size * nmemb;
	}

	IsSuccess Curl::CleanUp()
	{
		// もしcurlのポインタが存在するなら
		if (curl_) {
			// クリーンアップ処理を行う
			curl_easy_cleanup(curl_);
			curl_ = nullptr;
			return true;
		}
		return false;
	}

	Curl::Status Curl::Init()
	{
		if (*this) {
			// もし既に初期化されているなら､初期化に失敗する
			return Status::kUnknownError;
		}

		// CURLの初期化
		curl_ = curl_easy_init();

		if (not curl_) {
			// もし初期化に失敗したら
			return Status::kInitError;
		}

		return Status::kSuccess;


	}

	const std::string Curl::Status::StatusToString(const Status &status)
	{
		switch (status) {
		case Status::kSuccess:			return "Success";
		case Status::kInitError:		return "CURL初期化エラー";
		case Status::kSetOptionError:	return "CURLオプション設定エラー";
		case Status::kPerformError:		return "CURL実行エラー";
		case Status::kUnknownError:		return "CURL不明なエラー";
		default:						return "CURL不明なステータス";
		}
	}
}