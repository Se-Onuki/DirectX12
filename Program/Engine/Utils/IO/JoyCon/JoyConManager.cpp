#include "JoyConManager.h"

namespace SoLib {
	IsSuccess JoyConManager::Init()
	{
		// 接続されている任天堂のHIDデバイスの連結リストを取得。
		deviceList_ = SoLib::HidDeviceHolder::Generate(SoLib::JOYCON_IDs::NINTENDO_VENDOR_ID);

		// Joy-Con以外のデータを破棄
		deviceList_ = std::move(deviceList_.DrainFilter(SoLib::FuncFindJoyCon));

		// Joy-Conのデータの名付け
		auto findJoycon = deviceList_.FindDevice(SoLib::FuncFindJoyCon);


		if (findJoycon.size() < 2) { return false; }

		// Joy-Conの状態設定と振り分け
		for (const auto [name, device] : findJoycon) {
			// プロダクトID等を指定して、HID deviceをopenする。そうすると、そのhidデバイスの情報が載ったhid_deviceが帰ってくる。
			[[maybe_unused]] hid_device_ *dev = device.HidOpen();
			//// 今開いているデバイスのプロダクト名の取得。
			//std::wcout << L"vendor_id: " << device->vendor_id << std::endl;
			//std::wcout << L"manufacturer_string: " << device->manufacturer_string << std::endl;
			//std::wcout << L"product_string: " << device->product_string << std::endl;
			//std::wcout << L"product_id: " << device->product_id << std::endl;
			//std::cout << "name: " << name << std::endl;

			std::byte arg;
			arg = std::byte(0x1);
			SoLib::SendSubcommand(dev, std::byte(0x40u), { &arg,1 });
			arg = std::byte(0x30);
			SoLib::SendSubcommand(dev, std::byte(0x03u), { &arg,1 });

			// hid_set_nonblocking(dev, 1);

			if (name == "Joycon(L)") {
				devJoyL_ = dev;
			}
			else {
				devJoyR_ = dev;
			}
		}

		return true;
	}
}