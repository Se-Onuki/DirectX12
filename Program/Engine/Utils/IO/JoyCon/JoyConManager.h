#pragma once
#include "JoyConLoader.h"
#include "JoyCon.h"
#include "../../Success.h"

namespace SoLib {

	class JoyConInputParser {
	public:

		inline static constexpr std::array<int16_t, 3u> kOriginAngle_{
		std::bit_cast<int16_t>(uint16_t(0x000Eu)),
		std::bit_cast<int16_t>(uint16_t(0xFFDFu)),
		std::bit_cast<int16_t>(uint16_t(0xFFD0u)),
		};

		inline static constexpr int16_t kCalGyroCoeff_ = std::bit_cast<int16_t>(uint16_t(0x343Bu));

		struct MonoSensorData {
			std::array<int16_t, 3u> accel_;
			std::array<int16_t, 3u> gyro_;
		};

		void Calc(const float deltaTime) const;


	private:

		void Calc(const BinaryJoyConData *ref, JoyConBase *out) const;
		std::array<Math::Euler,3u> CalcRot(const BinaryJoyConData *ref) const;
	public:
		SoLib::JoyConUpdater *reference_ = nullptr;
		DoubleJoyCon *joycon_ = nullptr;
	};

	class JoyConManager {
	public:
		JoyConManager() = default;
		~JoyConManager() = default;

		IsSuccess Init();

		std::array<hid_device_ *, 2u> GetJoyConDevice() const { return { devJoyR_, devJoyL_ }; }

		//private:

		hid_device_ *devJoyR_ = nullptr;
		hid_device_ *devJoyL_ = nullptr;

		HidDeviceInfoRange deviceList_;

		DoubleJoyCon joycon_;

		JoyConInputParser inputParser_;
	};




}