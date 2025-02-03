#include "UIValue.h"
#include "../../DirectBase/Base/WinApp.h"
#include <string_view>
#include <string>
#include <iostream>
#include <regex>
#include <charconv>
namespace SoLib {

	// 入力文字列をパースし、計算結果を得る関数
	float ParseAndCalculate(const ::std::string &str, float kWindowPixel) {
		float result = 0.f;
		// 正規表現
		const static std::regex pattern(R"(([-+]?[0-9]*\.?[0-9]+)(%|px))");

		for (std::sregex_iterator it(str.begin(), str.end(), pattern), end; it != end; ++it) {
			auto &&match = *it;
			float number = 0.f;
			std::string unit = match[2];

			// 数値部分をfloatに変換
			auto parseResult = std::from_chars(&*match[1].first, &*match[1].second, number);
			if (parseResult.ec != std::errc()) {
				throw std::runtime_error("Failed to parse number");
			}

			// 単位に応じて値を加算
			if (unit == "%") {
				result += kWindowPixel * 0.01f * number;
			}
			else if (unit == "px") {
				result += number;
			}
		}

		return result;
	}


	namespace SoLib {

		Vector2 GenerateUIElement2(const std::string &xStr, const std::string &yStr)
		{
			Vector2 result{
				.x = ParseAndCalculate(xStr, static_cast<float>(WinApp::kWindowWidth)),
				.y = ParseAndCalculate(yStr, static_cast<float>(WinApp::kWindowHeight))
			};

			return result;
		}

	}
}