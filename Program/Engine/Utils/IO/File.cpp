/// @file File.cpp
/// @brief ファイルクラス
/// @author ONUKI seiya
#include "File.h"

SoLib::IO::Value &SoLib::IO::Value::convert() {

	if (std::holds_alternative<SoLib::Text::ConstString<12u>>(item_)) {
		const std::string strValue = std::get<SoLib::Text::ConstString<12u>>(item_).data();

		// 数字と小数点以外の文字を検知
		if (strValue.find_first_not_of("-0123456789.") != std::string::npos) {
			return *this;
		}
		// "."が含まれているか確認
		if (strValue.find('.') != std::string::npos) {
			item_ = std::stof(strValue); // floatに変換
		}
		else {
			item_ = std::stoi(strValue); // int32_tに変換
		}
	}

	return *this;
}
