#include "JoyConManager.h"
#include "../../../DirectBase/Input/Input.h"

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

		inputParser_.joycon_ = &joycon_;

		return true;
	}

	void JoyConInputParser::Update(const float)
	{
		std::vector<std::array<Math::Euler, 3u>> eulerRight;
		std::vector<std::array<Math::Euler, 3u>> eulerLeft;

		std::vector<float> rDelta;
		std::vector<float> lDelta;

		// 送信される情報の差分
		static constexpr float kSendCount = 200.f * 3;

		do {
			reference_->Update();
			const auto &[right, left] = reference_->GetJoyConRL();
			if (right) {
				eulerRight.push_back(CalcRot(&right));
				rDelta.push_back(right.GetTimeSpan() / kSendCount);
			};
			if (left) {
				eulerLeft.push_back(CalcRot(&left));
				lDelta.push_back(left.GetTimeSpan() / kSendCount);
			}
			if (not right or not left) { break; }

		} while (true);

		auto input = SolEngine::DirectInput::GetInstance();

		if (input->IsTrigger(DIK_L)) {
			static_cast<Vector3 &>(rotateBias_[0]) = Vector3::zero;
		}
		auto eulerRange = std::views::join(eulerRight);
		for (int32_t i = 0; auto & rot : eulerRange) {
			if (input->IsTrigger(DIK_L)) {
				static_cast<Vector3 &>(rotateBias_[0]) += rot;
			}
			joycon_->joyConR_.transform_.rotate_.RK4(rot, rDelta[i++ / 3]);
		}
		if (input->IsTrigger(DIK_L)) {
			static_cast<Vector3 &>(rotateBias_[0]) /= static_cast<float>(eulerRight.size() * 3.f);
		}
		eulerRange = std::views::join(eulerLeft);
		for (int32_t i = 0; auto & rot : eulerRange) {
			joycon_->joyConL_.transform_.rotate_.RK4(rot, lDelta[i++ / 3]);
		}

	}

	void JoyConInputParser::Calc(const BinaryJoyConData *ref, JoyConBase *out) const
	{

		// 情報の分解

		// 書き込み先
		std::array<MonoSensorData, 3u> data;
		// 元のセンサーデータ
		auto refData = ref->GetSensor();
		// 書き込み
		std::memcpy(data.data(), refData.data(), refData.size());

		static constexpr float kRotCalc = 4588.f / 65535;

		Quaternion angleAccel = Quaternion::Identity;

		for (auto &item : std::views::reverse(data)) {

			Math::Euler rot{};

			for (uint32_t i = 0; i < 3u; i++) {

				float gyro_cal_coeff = (float)(936.f / (float)(kCalGyroCoeff_ - kOriginAngle_[i]));
				//float sign = std::copysignf(1.f, int16_t(item.gyro_[i] - kOriginAngle_[i]));
				/*rot.begin()[i] =(sign * std::clamp(int16_t(std::abs(item.gyro_[i]) - 75), int16_t(0), (std::numeric_limits<int16_t>::max)())) * kRotCalc * (1 / 3600.f);*/
				rot.begin()[i] = (item.gyro_[i] - rotateBias_[0].begin()[i]) * gyro_cal_coeff / 3600.f / 3;



			}
			//rot.x *= -1.f;
			auto tmp = Quaternion::Create(rot);
			//tmp = Quaternion{ -tmp.x, -tmp.y, tmp.z, tmp.w }.Normalize();

			(angleAccel *= tmp).Normalize();
		}


		(out->transform_.rotate_ *= /*{ -angleAccel.x, -angleAccel.y, angleAccel.z, angleAccel.w }*/ angleAccel).Normalize();
	}
	std::array<Math::Euler, 3u> JoyConInputParser::CalcRot(const BinaryJoyConData *ref) const
	{
		std::array<Math::Euler, 3u> result;

		// 情報の分解

		// 書き込み先
		std::array<MonoSensorData, 3u> data;
		// 元のセンサーデータ
		auto refData = ref->GetSensor();
		// 書き込み
		std::memcpy(data.data(), refData.data(), refData.size());

		static constexpr float kRotCalc = 4588.f / 65535;

		Quaternion angleAccel = Quaternion::Identity;

		for (auto itr = result.begin(); auto & item : std::views::reverse(data)) {

			Math::Euler &rot = *itr++;

			for (uint32_t i = 0; i < 3u; i++) {

				float gyro_cal_coeff = (float)(936.f * Angle::Dig2Rad / (float)(kCalGyroCoeff_));
				//float sign = std::copysignf(1.f, int16_t(item.gyro_[i]));
				///*rot.begin()[i] =(sign * std::clamp(int16_t(std::abs(item.gyro_[i]) - 75), int16_t(0), (std::numeric_limits<int16_t>::max)())) * kRotCalc * (1 / 3600.f);*/
				rot.begin()[i] = ((item.gyro_[i])) * gyro_cal_coeff;
			}
			rot.x *= -1.f;
			rot.y *= -1.f;
			//auto tmp = Quaternion::Create(rot);
			//tmp = Quaternion{ -tmp.x, -tmp.y, tmp.z, tmp.w }.Normalize();

			static_cast<Vector3 &>(rot) -= static_cast<const Vector3&>(rotateBias_[0]);

			//(angleAccel *= tmp).Normalize();
		}

		return result;
	}
}