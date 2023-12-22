#pragma once
#include <vector>
#include <stdexcept>

namespace SoLib {
	namespace Containers {

		template<typename T>
		class Array2D {
		public:
			// コンストラクタ
			Array2D(size_t rows = 0u, size_t cols = 0u) { Resize(rows, cols); }

			// 行数を取得
			size_t GetRows() const {
				return rows_;
			}

			// 列数を取得
			size_t GetCols() const {
				return cols_;
			}

			// 指定した位置の要素を取得
			T &at(size_t row, size_t col) {
				if (row >= rows_ || col >= cols_) {
					throw std::out_of_range("添え字が範囲外に出ています");
				}
				return data_[row][col];
			}

			const T &at(size_t row, size_t col) const {
				if (row >= rows_ || col >= cols_) {
					throw std::out_of_range("添え字が範囲外に出ています");
				}
				return data_[row][col];
			}

			// 行数を変更
			void ResizeRows(size_t newRows) {
				if (newRows == rows_) { return; }

				data_.resize(newRows, std::vector<T>(cols_));
				rows_ = newRows;
			}

			// 列数を変更
			void ResizeCols(size_t newCols) {
				if (newCols == cols_) { return; }

				for (auto &row : data_) {
					row.resize(newCols);
				}
				cols_ = newCols;
			}

			// 行と列のサイズを同時に変更
			void Resize(size_t newRows, size_t newCols) {
				ResizeRows(newRows);
				ResizeCols(newCols);
			}

			auto &get() { return data_; }
			const auto &get()const { return data_; }


			class iterator {
			public:
				iterator(Array2D<T> *parent, size_t row, size_t col) : pParent_(parent), rowIt_(row), colIt_(col), kMaxRow_(parent->GetRows()), kMaxCol_(parent->GetCols()) {}

				bool operator==(const iterator &other) const {
					return rowIt_ == other.rowIt_ && colIt_ == other.colIt_;
				}

				bool operator!=(const iterator &other) const {
					return !(*this == other);
				}

				iterator &operator++() {
					++colIt_;
					if (colIt_ == kMaxCol_) {
						++rowIt_;
						colIt_ = 0;
					}
					return *this;
				}

				T &operator*() const {
					return pParent_->at(rowIt_, colIt_);
				}

			private:
				size_t rowIt_;
				size_t colIt_;

				size_t kMaxRow_;
				size_t kMaxCol_;

				Array2D<T> *pParent_;

			};

			iterator begin() {
				return iterator(this, 0, 0);
			}

			iterator end() {
				return iterator(this, rows_, 0);
			}
		private:
			size_t rows_;
			size_t cols_;
			std::vector<std::vector<T>> data_;

		};

	}
}