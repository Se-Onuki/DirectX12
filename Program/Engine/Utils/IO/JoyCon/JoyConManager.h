#pragma once
#include "JoyConLoader.h"
#include "JoyCon.h"
#include "../../Success.h"

namespace SoLib {

	/// @brief Joy-Conの入力の分解
	class JoyConInputParser {
	public:

		/// @brief Joy-Conの左右
		enum class JoyconRL {
			kR_ = 0,
			kL_ = 1,
		};


		/// @brief 基礎回転量(初期のブレ)
		inline static constexpr std::array<int32_t, 3u> kOriginAngle_
		{
			std::bit_cast<int16_t>(uint16_t(0x000Eu)),
			std::bit_cast<int16_t>(uint16_t(0xFFDFu)),
			std::bit_cast<int16_t>(uint16_t(0xFFD0u)),
		};

		/// @brief ジャイロの回転倍率
		inline static constexpr int16_t kCalGyroCoeff_ = std::bit_cast<int16_t>(uint16_t(0x343Bu));

		/// @brief センサーのデータ配置の単体(3回繰り返す)
		struct MonoSensorData {
			std::array<int16_t, 3u> accel_;
			std::array<int16_t, 3u> gyro_;
		};

		/// @brief 更新処理
		/// @param[in] deltaTime 時間差分
		void Update(const float deltaTime);


	private:
		/// @brief 
		/// @param[in] ref 参照元
		/// @param[in] bias ズレ
		/// @param[in, out] updateBias ズレの更新書き込み先
		/// @return 
		std::array<Math::Euler, 3u> CalcRot(const BinaryJoyConData *ref, const std::array<int16_t, 3u> &bias, std::array<int32_t, 3u> &updateBias) const;
	public:

		/// @brief ズレ
		std::array<std::array<int32_t, 3u>, 2u> bias_{ kOriginAngle_, kOriginAngle_ };

		/// @brief 回転のズレ
		std::array<std::array<int16_t, 3u>, 2u> rotateBias_{
			std::array<int16_t, 3u>{
				std::bit_cast<int16_t>(uint16_t(0x000Eu)),
				std::bit_cast<int16_t>(uint16_t(0xFFDFu)),
				std::bit_cast<int16_t>(uint16_t(0xFFD0u)),
			},
			std::array<int16_t, 3u>{
				std::bit_cast<int16_t>(uint16_t(0x000Eu)),
				std::bit_cast<int16_t>(uint16_t(0xFFDFu)),
				std::bit_cast<int16_t>(uint16_t(0xFFD0u)),
			}
		};
		/// @brief ズレの更新用の計算時間
		int32_t isBiasUpdateCount_ = 10;
		/// @brief ズレの更新のためのカウンタ
		std::array<int16_t, 2u> biasCount_;

		/// @brief 更新処理の情報源
		SoLib::JoyConUpdater *reference_ = nullptr;
		/// @brief Joy-Conのデータ
		DoubleJoyCon *joycon_ = nullptr;
	};

	class JoyConManager {
	public:
		JoyConManager() = default;
		~JoyConManager() = default;

		/// @brief 初期化処理
		/// @return 初期化に成功したか｡
		IsSuccess Init();

		/// @brief Joy-Conのデバイスの取得
		/// @return Joy-Conのデバイスの右､左
		std::array<hid_device_ *, 2u> GetJoyConDevice() const { return { devJoyR_, devJoyL_ }; }

		//private:

		/// @brief Joy-Conのデバイス
		hid_device_ *devJoyR_ = nullptr;
		/// @brief Joy-Conのデバイス
		hid_device_ *devJoyL_ = nullptr;

		/// @brief デバイスの格納されたコンテナ(所有権)
		HidDeviceInfoRange deviceList_;

		/// @brief Joy-Conのデータ
		DoubleJoyCon joycon_;

		// Joy-Conのデータの分解処理
		JoyConInputParser inputParser_;
	};




}