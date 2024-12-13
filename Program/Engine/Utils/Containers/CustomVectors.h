/// @file CustomVectors.h
/// @brief ベクターの定義
/// @author ONUKI seiya
#pragma once
#include <cstdint>
#include <array>
#include <algorithm>

#pragma warning(push)  // 現在の警告のステータスを保存する
#pragma warning(disable : 4201)  // C4201警告を無視する

template<typename Contain>
struct CustomVector2 {
	union {
		std::array<Contain, 2u> arr;
		Contain x, y;
	};

	CustomVector2() = default;
	CustomVector2(std::initializer_list<float> values) {
		std::copy(values.begin(), values.begin() + 2u, arr.begin());
	}
};


template<typename Type>
struct CustomVector3 {
	static size_t size() { return 3u; }
	union {
		std::array<Type, 3u> arr;
		Type x, y, z;
	};


	CustomVector3() = default;
	CustomVector3(const Type& xV, const Type &yV, const Type &zV) {
		arr[0] = xV;
		arr[1] = yV;
		arr[2] = zV;
	}

	CustomVector3(const CustomVector3 &) = default;
	CustomVector3 &operator=(const CustomVector3 &) = default;

	CustomVector3(CustomVector3 &&) = default;
	CustomVector3 &operator=(CustomVector3 &&) = default;

};






#pragma warning(pop)  // 以前の警告のステータスに戻す
