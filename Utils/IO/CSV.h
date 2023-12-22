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

			CsvData &operator=(const std::string &str);

			CsvData &operator=(const std::vector<std::string> &data) {
				const size_t length = data.size();

				this->resize(length);

				for (size_t i = 0u; i < length; i++) {
					cellList_[i] = data[i];
				}
				return *this;
			}

			void resize(const size_t size) {
				if (size_ != size) {
					size_ = size;
					cellList_.resize(size);
				}
			}

			ValueItem &operator[](const size_t index) { return cellList_.at(index); }

			const ValueItem &operator[](const size_t index) const { return cellList_.at(index); }

			const auto &size() const { return size_; }

			auto begin() { return cellList_.begin(); }
			auto begin() const { return cellList_.begin(); }
			auto cbegin() const { return cellList_.begin(); }

			auto end() { return cellList_.end(); }
			auto end() const { return cellList_.end(); }
			auto cend() const { return cellList_.end(); }

		private:
			std::vector<Value> cellList_;
			size_t size_;
			CSV *const pParent_;
		};

		class CSV final {
		public:

			using Map = std::vector<CsvData>;

			CSV() = default;
			CSV(const File &file);
			~CSV() = default;

			CsvData &operator[](const size_t index);

		private:

			Map data_;

		};
	}
}