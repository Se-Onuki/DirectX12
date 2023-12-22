#pragma once
#include <iostream>
#include <vector>
#include <stdexcept>

template<typename T>
class Dynamic2DArray {
private:
	std::vector<std::vector<T>> data;
	size_t rows;
	size_t cols;

public:
	// コンストラクタ
	Dynamic2DArray(size_t initialRows, size_t initialCols) : rows(initialRows), cols(initialCols) {
		data.resize(rows, std::vector<T>(cols));
	}

	// 行数を取得
	size_t getRows() const {
		return rows;
	}

	// 列数を取得
	size_t getCols() const {
		return cols;
	}

	// 指定した位置の要素を取得
	T &at(size_t row, size_t col) {
		if (row >= rows || col >= cols) {
			throw std::out_of_range("Index out of range");
		}
		return data[row][col];
	}

	const T &at(size_t row, size_t col) const {
		if (row >= rows || col >= cols) {
			throw std::out_of_range("Index out of range");
		}
		return data[row][col];
	}

	// 行数を変更
	void resizeRows(size_t newRows) {
		if (newRows == rows) return;

		data.resize(newRows, std::vector<T>(cols));
		rows = newRows;
	}

	// 列数を変更
	void resizeCols(size_t newCols) {
		if (newCols == cols) return;

		for (auto &row : data) {
			row.resize(newCols);
		}
		cols = newCols;
	}

	// 行と列のサイズを同時に変更
	void resize(size_t newRows, size_t newCols) {
		resizeRows(newRows);
		resizeCols(newCols);
	}


	class iterator {
	private:
		typename std::vector<std::vector<T>>::iterator rowIt;
		typename std::vector<T>::iterator colIt;

	public:
		iterator(typename std::vector<std::vector<T>>::iterator row, typename std::vector<T>::iterator col)
			: rowIt(row), colIt(col) {}

		bool operator==(const iterator &other) const {
			return rowIt == other.rowIt && colIt == other.colIt;
		}

		bool operator!=(const iterator &other) const {
			return !(*this == other);
		}

		iterator &operator++() {
			++colIt;
			if (colIt == rowIt->end()) {
				++rowIt;
				colIt = rowIt->begin();
			}
			return *this;
		}

		T &operator*() const {
			return *colIt;
		}
	};

	iterator begin() {
		return iterator(data.begin(), data.begin()->begin());
	}

	iterator end() {
		return iterator(data.end(), data.end()->end());
	}
};