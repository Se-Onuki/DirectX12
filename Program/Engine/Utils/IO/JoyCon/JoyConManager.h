#pragma once
#include "JoyConLoader.h"
#include "JoyCon.h"
#include "../../Success.h"

namespace SoLib {

	class JoyConManager {
	public:
		JoyConManager() = default;
		~JoyConManager() = default;

		IsSuccess Init();

		std::array<hid_device_ *, 2u> GetJoyConDevice() const { return { devJoyR_, devJoyL_ }; }

	private:

		HidDeviceInfoRange deviceList_;

		hid_device_ *devJoyR_ = nullptr;
		hid_device_ *devJoyL_ = nullptr;
	};


}