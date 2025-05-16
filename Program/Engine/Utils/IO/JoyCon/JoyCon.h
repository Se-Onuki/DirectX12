#pragma once
#include <functional>
#include <string>
#include <span>
#include <array>
#include <memory>
#include <bit>
#include <bitset>

#include "hidapi.h"

#include "../../Math/Transform.h"
#include "../HID/HidDeviceHolder.h"


#include "JoyConLoader.h"

namespace SoLib {

	/// @brief Joy-Conの共通部分
	class JoyConBase {
	public:
		JoyConBase() = default;
		~JoyConBase() = default;

		//private:

		/// @brief 加速度
		Vector3 accel_ = {};
		/// @brief 速度
		Vector3 velocity_ = {};

		/// @brief 姿勢
		SimpleTransformQuaternion transform_;

	};


	class JoyConR : public JoyConBase
	{
	public:
		JoyConR() = default;
		~JoyConR() = default;

		/// @brief 押したかどうか
		/// @param[in] button ボタン
		/// @return 押したならTrue
		bool IsPress(BinaryJoyConData::JoyButtonR button) const;

		/// @brief ボタンのバイナリ
		uint16_t button_;

	};

	class JoyConL : public JoyConBase
	{
	public:
		JoyConL() = default;
		~JoyConL() = default;
		/// @brief 押したかどうか
		/// @param[in] button ボタン
		/// @return 押したならTrue
		bool IsPress(BinaryJoyConData::JoyButtonL button) const;

	private:
		/// @brief ボタンのバイナリ
		uint16_t button_;

	};

	class DoubleJoyCon {
	public:

		DoubleJoyCon() = default;


	//private:

		/// @brief 右Joy-Con
		JoyConR joyConR_;
		/// @brief 左Joy-Con
		JoyConL joyConL_;

	};

}