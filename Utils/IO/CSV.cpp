#include "CSV.h"

SoLib::IO::CSV::CSV(const File &file) {

	std::stringstream fileData;
	fileData << file.GetData().rdbuf();

	data_.clear();
	std::string line = "";
	while (std::getline(fileData, line)) {
		CsvData csvData(this, 0);
		//csvData = line;
		data_.push_back(csvData);
	}
}

SoLib::IO::CsvData &SoLib::IO::CSV::operator[](const size_t index) {
	return this->data_.at(index);
}

SoLib::IO::CsvData &SoLib::IO::CsvData::operator=(const std::string &str) {
	str;
	// TODO: return ステートメントをここに挿入します
	return *this;
}
