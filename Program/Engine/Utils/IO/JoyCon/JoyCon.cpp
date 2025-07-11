#include "JoyCon.h"


namespace SoLib {


	bool JoyConR::IsPress(BinaryJoyConData::JoyButtonR button) const
	{
		return button_ & static_cast<uint8_t>(button);
	}

	bool JoyConL::IsPress(BinaryJoyConData::JoyButtonL button) const
	{
		return button_ & static_cast<uint8_t>(button);
	}

}