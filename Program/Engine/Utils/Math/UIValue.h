#pragma once
#include <string>

#include "Vector2.h"
float ParseAndCalculate(const std::string &str, float kWindowPixel);

namespace SoLib {

	Vector2 GenerateUIElement2(const std::string &x, const std::string &y);

}