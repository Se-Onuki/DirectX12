/// @file File.h
/// @brief ファイルクラス
/// @author ONUKI seiya
#pragma once
#include <string>
#include <fstream>
#include <sstream>

#include <variant>
#include <optional>
#include "../Text/ConstString.h"

namespace SoLib {

	namespace IO {
		
		/// @brief ファイルの値を格納する型
		using ValueItem = std::variant<int32_t, float, SoLib::Text::ConstString<12u>>;
		class Value final {
		public:
			Value(const SoLib::Text::ConstString<12u> &str = {}) : item_(str) {};
			Value(const std::string &value) { *this = value; }
			Value(const ValueItem &value) : item_(value) {}
			~Value() = default;

			Value &operator=(const std::string &value) { item_ = SoLib::Text::ConstString<12u>{ value.c_str() }; return *this; }
			Value &operator=(const ValueItem &value) { item_ = value; return *this; }
			Value &operator=(ValueItem &&value) { item_ = value; return *this; }

			Value &convert();

			operator ValueItem &() { return item_; }
			operator const ValueItem &() const { return item_; }

		private:
			ValueItem item_;
		};


		class File {
		public:
			File() = default;
			// コンストラクタ
			File(const std::string &filePath) {
				Load(filePath);
			}

			/// @brief ファイルを読み込む
			/// @param filePath 読み込むファイルのパス
			/// @return 読み込みに成功したか
			bool Load(const std::string &filePath) {
				std::ifstream file(filePath);
				if (not file) {
					// std::cerr << "ファイルが開けません。" << std::endl;
					return false;
				}

				data_ << file.rdbuf();
				file.close();

				filePath_ = filePath;
				return true;
			}

			// データを取得する
			auto &GetData() { return data_; }
			const auto &GetData() const { return data_; }
			/// @brief ファイルパスを取得する
			const std::string &GetFilePath() const { return filePath_; }

			/// @brief ファイルが読み込まれているか
			operator bool() const { return filePath_.size(); }

			/// @brief sstreamとして振る舞う
			operator std::stringstream &() { return data_; }

		private:
			std::string filePath_;
			std::stringstream data_;
		};
	}
}