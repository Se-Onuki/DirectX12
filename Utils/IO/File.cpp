#include "File.h"

void SoLib::IO::Value::convert() {

	if (std::holds_alternative<SoLib::ConstString<12u>>(item_)) {
		const std::string strValue = std::get<SoLib::ConstString<12u>>(item_).data();

		// "."が含まれているか確認
		if (strValue.find('.') != std::string::npos) {
			item_ = std::stof(strValue); // floatに変換
		}
		else {
			item_ = std::stoi(strValue); // int32_tに変換
		}
	}

}
