/// @file CSV.h
/// @brief CSV形式で保存する型
/// @author ONUKI seiya
#pragma once
#include <string>
#include <vector>
#include "File.h"

namespace SoLib {
	namespace IO {

		class CSV;

		class CsvRow {
		public:
			CsvRow(CSV *parent, const uint32_t index) : pParent_(parent) { index; }
			~CsvRow() = default;

		private:
			CSV *pParent_ = nullptr;
		};

		class CsvColumn {
		public:
			CsvColumn(CSV *parent, const uint32_t index) : pParent_(parent) { index; }
			~CsvColumn() = default;
		private:
			CSV *pParent_ = nullptr;
		};

		class CsvData {

		public:
			CsvData(CSV *parent, size_t size) : pParent_(parent) { this->resize(size); }

			CsvData &operator=(const std::string &line);

			CsvData &operator=(const std::vector<std::string> &data) {
				const size_t length = data.size();

				this->resize(length);

				for (size_t i = 0u; i < length; i++) {
					cellList_[i] = data[i];
				}
				return *this;
			}

			/// @brief CSVの行のサイズを変更する
			/// @param size 新しいサイズ
			void resize(const size_t size) {
				if (size_ != size) {
					size_ = size;
					cellList_.resize(size);
				}
			}

			std::string &operator[](const size_t index) { return cellList_.at(index); }

			const std::string &operator[](const size_t index) const { return cellList_.at(index); }

			const auto &size() const { return size_; }

			auto begin() { return cellList_.begin(); }
			auto begin() const { return cellList_.begin(); }
			auto cbegin() const { return cellList_.begin(); }

			auto end() { return cellList_.end(); }
			auto end() const { return cellList_.end(); }
			auto cend() const { return cellList_.end(); }

		private:
			std::vector<std::string> cellList_;
			size_t size_;
			CSV *const pParent_;
		};

		class CSV final {
		public:

			using Map = std::vector<CsvData>;

			CSV() = default;
			CSV(const File &file);
			~CSV() = default;

			/// @brief CSVファイルを読み込む
			/// @param file CSVファイル
			CSV &operator=(const File &file);

			/// @brief 指定した行を取得
			/// @param index 行数
			/// @return 指定した行のデータ
			CsvData &operator[](const size_t index);

			Map &Get() { return data_; }
			const Map &Get() const { return data_; }

			auto begin() { return data_.begin(); }
			auto begin() const { return data_.cbegin(); }
			auto cbegin() const { return data_.cbegin(); }

			auto end() { return data_.end(); }
			auto end() const { return data_.cend(); }
			auto cend() const { return data_.cend(); }

			size_t GetWidth() const { return width_; }
			size_t GetHeight() const { return height_; }

		private:
			// 横幅
			size_t width_;
			// 縦幅
			size_t height_;

			Map data_;

		};

		/// @brief CSVを出力する
		/// @param stream 出力先
		/// @param value 出力するCSV
		/// @return 出力先
		inline std::ostream &operator<<(std::ostream &stream, const CSV &value) {
			for (auto &line : value) {
				for (auto &item : line) {
					stream << item << ", ";
				}
				stream.seekp(-2, std::ios_base::cur);
				stream << "\n";
			}
			return stream;
		}
	}
}