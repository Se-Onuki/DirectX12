#pragma once
#include <string>
#include <fstream>
#include <sstream>

#include <variant>
#include <optional>
#include "../ConstString.h"

namespace SoLib {

	namespace IO {

		using ValueItem = std::variant<int32_t, float, SoLib::ConstString<12u>>;
		class Value final {
		public:
			Value() : item_(int32_t{}) {}
			~Value() = default;

			Value &operator=(const std::string &value) { item_ = SoLib::ConstString<12u>{ value.c_str() }; return *this; }
			Value &operator=(const ValueItem &value) { item_ = value; return *this; }
			Value &operator=(ValueItem &&value) { item_ = value; return *this; }

			void convert();

			operator ValueItem &() { return item_; }
			operator const ValueItem &() const { return item_; }

		private:
			ValueItem item_;
		};


		class File {
		public:
			// コンストラクタ
			File(const std::string &filePath) {
				std::ifstream file(filePath);
				if (not file) {
					// std::cerr << "ファイルが開けません。" << std::endl;
					return;
				}

				data_ << file.rdbuf();
				file.close();

				filePath_ = filePath;
			}

			// データを取得する
			auto &GetData() { return data_; }
			const auto &GetData() const { return data_; }
			const std::string &GetFilePath() const { return filePath_; }

		private:
			std::string filePath_;
			std::stringstream data_;
		};
	}
}