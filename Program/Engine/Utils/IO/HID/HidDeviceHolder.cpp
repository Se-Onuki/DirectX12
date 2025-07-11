#include "HidDeviceHolder.h"
// HIDAPIのヘッダ
#include "hidapi.h"

#include <windows.h>

namespace SoLib {
	HidDeviceInfoRange::HidDeviceInfoRange(HidDeviceInfoRange &&range)
	{
		clear();

		begin_ = range.begin_;
		range.begin_ = nullptr;
	}
	HidDeviceInfoRange &HidDeviceInfoRange::operator=(HidDeviceInfoRange &&range)
	{
		clear();

		begin_ = range.begin_;
		range.begin_ = nullptr;
		return *this;
	}
	HidDeviceInfoRange::~HidDeviceInfoRange()
	{
		clear();
	}

	void HidDeviceInfoRange::clear()
	{
		if (not begin_.itr_) { return; }
		hid_free_enumeration(begin_.itr_.info_);
		begin_ = nullptr;
	}

	void HidDeviceInfoRange::push_front(HidDeviceInfoItrator itr)
	{
		// 現在の先頭をitrの次の値として格納して､現在のbeginと入れ替える
		// itrの次のアドレスに現在のbeginを入れる
		itr.itr_.info_->next = begin_.itr_.info_;
		// 現在のbegin_にitrを入れる
		begin_ = itr;
	}

	HidDeviceInfoRange HidDeviceInfoRange::DrainFilter(std::string(*func)(const HidDeviceInfo))
	{

		HidDeviceInfoRange result;	// 結果

		HidDeviceInfoItrator itr = begin_;		// 開始地点のイテレータ
		HidDeviceInfoItrator prevItr = nullptr;	// 一つ前のイテレータ

		while (true) {
			// もしイテレータが空(nullptr)なら終わる
			if (not itr) {
				break;
			}
			// イテレータが条件に合致するかを検知
			auto name = func(*itr);
			// 名前があるなら真
			if (not name.empty()) {
				// 現在のイテレータを孤立させる
				// 次のイテレータを作成する
				HidDeviceInfoItrator next = itr.itr_.info_->next;

				// もし前のイテレータがあるなら､今の次のイテレータの値を入れる｡
				if (prevItr) {
					prevItr.itr_.info_->next = next.itr_.info_;
					// そしてそれを孤立させる
					itr.itr_.info_->next = nullptr;
				}

				// そのデータをresultに格納
				result.push_front(itr);

				// 次のイテレータを今のイテレータにする
				itr = next;

				// 過去のイテレータが無い(先頭)なら､begin_も更新する
				if (not prevItr) {
					begin_ = next;
				}

			}
			// 名前がないなら､次に進む
			else {
				prevItr = itr;
				itr++;
			}

		}

		return std::move(result);
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