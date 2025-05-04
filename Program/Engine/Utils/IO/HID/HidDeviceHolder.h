#pragma once
#include <list>
#include <string>
#include <functional>
#include <set>

// HIDデバイスのリスト
struct hid_device_info;
#include "hidapi.h"

namespace SoLib {

	class DeviceDataBase {
	public:

	};

	class HidDeviceHolder;
	class HidDeviceInfoRange;

	class HidDeviceInfo {
		friend HidDeviceInfoRange;
	public:
		HidDeviceInfo() = default;
		HidDeviceInfo(const HidDeviceInfo &) = default;
		HidDeviceInfo(hid_device_info *const info) : info_(info) {}
		HidDeviceInfo &operator=(const HidDeviceInfo &) = default;

		operator const hid_device_info *() const {
			return info_;
		}

		const hid_device_info &operator*() const {
			return *info_;
		}

		const hid_device_info *operator->() const {
			return info_;
		}

		hid_device *HidOpen() const {
			return hid_open(info_->vendor_id, info_->product_id, info_->serial_number);
		}

	private:
		hid_device_info *info_ = nullptr;
	};

	class HidDeviceInfoItrator {
		friend HidDeviceInfoRange;
	public:
		HidDeviceInfoItrator() = default;
		HidDeviceInfoItrator(hid_device_info *itr) : itr_(itr) {};
		HidDeviceInfoItrator(const HidDeviceInfoItrator &itr) = default;
		HidDeviceInfoItrator &operator=(const HidDeviceInfoItrator &itr) = default;

		bool operator==(const HidDeviceInfoItrator &) const = default;

		HidDeviceInfoItrator &operator++() {
			itr_ = itr_->next;
			return *this;
		}
		HidDeviceInfoItrator operator++(int32_t) const {
			return itr_->next;
		}

		const HidDeviceInfo operator->() const {
			return itr_;
		}

		const HidDeviceInfo operator*() const {
			return itr_;
		}


	private:
		HidDeviceInfo itr_ = nullptr;
	};

	class HidDeviceInfoRange {
	public:

		HidDeviceInfoRange() = default;

		HidDeviceInfoRange(const HidDeviceInfoRange &) = delete;
		HidDeviceInfoRange(HidDeviceInfoRange &&);

		HidDeviceInfoRange &operator=(const HidDeviceInfoRange &) = delete;
		HidDeviceInfoRange &operator=(HidDeviceInfoRange &&);

		HidDeviceInfoRange(const HidDeviceInfoItrator begin) : begin_(begin) {}
		~HidDeviceInfoRange();

		HidDeviceInfoItrator begin() const { return begin_; }
		HidDeviceInfoItrator end() const { return nullptr; }

		std::list<std::pair<std::string, const HidDeviceInfo>> FindDevice(std::string(*)(const HidDeviceInfo)) const;

	private:
		HidDeviceInfoItrator begin_;
	};

	class HidDeviceHolder {
	public:
		HidDeviceHolder() = default;

		static HidDeviceInfoRange Generate(uint16_t venderID = 0, uint16_t productID = 0);


	private:
	};


}