#include "CSV.h"

SoLib::IO::CSV::CSV(const File &file) {
	*this = file;
}

SoLib::IO::CSV &SoLib::IO::CSV::operator=(const File &file) {
	std::stringstream fileData;
	fileData << file.GetData().rdbuf();

	size_t maxLength{};

	data_.clear();
	std::string line = "";
	while (std::getline(fileData, line)) {
		CsvData csvData(this, 0);
		csvData = line;
		maxLength = (std::max)(maxLength, csvData.size());
		data_.push_back(csvData);
	}

	// サイズを統一する
	for (auto &item : data_) {
		item.resize(maxLength);
	}
	width_ = maxLength;
	height_ = data_.size();

	return *this;
}

SoLib::IO::CsvData &SoLib::IO::CSV::operator[](const size_t index) {
	return this->data_.at(index);
}

SoLib::IO::CsvData &SoLib::IO::CsvData::operator=(const std::string &line) {
	cellList_.clear();

	std::string cell = "";
	std::stringstream lineBuff = std::stringstream(line);
	while (std::getline(lineBuff, cell, ',')) {
		// 先頭の空白文字を削除
		size_t start = cell.find_first_not_of(" \t");
		if (start != std::string::npos) {
			cell = cell.substr(start);
		}

		// 終端の空白文字を削除
		size_t end = cell.find_last_not_of(" \t");
		if (end != std::string::npos) {
			cell = cell.substr(0, end + 1);
		}
		cellList_.push_back(cell);
	}
	size_ = cellList_.size();
	return *this;
}
