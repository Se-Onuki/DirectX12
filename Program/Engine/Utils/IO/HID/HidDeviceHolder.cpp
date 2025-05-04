#include "HidDeviceHolder.h"
// HIDAPIのヘッダ
#include "hidapi.h"

#include <windows.h>

namespace SoLib {
	HidDeviceInfoRange::HidDeviceInfoRange(HidDeviceInfoRange &&range)
	{
		begin_ = range.begin_;
		range.begin_ = nullptr;
	}
	HidDeviceInfoRange &HidDeviceInfoRange::operator=(HidDeviceInfoRange &&range)
	{
		begin_ = range.begin_;
		range.begin_ = nullptr;
		return *this;
	}
	HidDeviceInfoRange::~HidDeviceInfoRange()
	{
		if (not begin_.itr_) { return; }
		hid_free_enumeration(begin_.itr_.info_);
	}

	std::list<std::pair<std::string, const HidDeviceInfo>> HidDeviceInfoRange::FindDevice(std::string(*func)(const HidDeviceInfo)) const
	{
		std::list<std::pair<std::string, const HidDeviceInfo>> result;

		for (auto device : *this) {

			// 名前を検証する
			std::string name = func(device);
			// 文字がなかったら
			if (name.empty()) { continue; }
			// 文字があったらそれを保存する
			result.push_back({ std::move(name), device });

		}

		return result;
	}

	HidDeviceInfoRange HidDeviceHolder::Generate(uint16_t venderID, uint16_t productID)
	{
		HidDeviceInfoRange result{};

		result = { hid_enumerate(venderID, productID) };

		return std::move(result);
	}


}