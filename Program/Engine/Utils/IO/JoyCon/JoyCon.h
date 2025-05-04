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


	class JoyConBase {
	public:
		JoyConBase() = default;
		~JoyConBase() = default;

	private:

		SimpleTransformQuaternion transform_;

	};


	class JoyConR
	{
	public:
		JoyConR() = default;
		~JoyConR() = default;

		bool IsPress(BinaryJoyConData::JoyButtonR) const;

		uint16_t button_;

	};

	class JoyConL
	{
	public:
		JoyConL() = default;
		~JoyConL() = default;

		bool IsPress(BinaryJoyConData::JoyButtonL) const;

	private:

		uint16_t button_;

	};

	class DoubleJoyCon {
	public:

		DoubleJoyCon() = default;


	private:

		JoyConR joyConR_;
		JoyConL joyConL_;

	};

}