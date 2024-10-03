#pragma once
#include "Vector3.h"
#include "../Containers/WrappingClass.h"

struct /*alignas(16)*/ AlignasVector3 : public SoLib::Containers::WrappingClass<AlignasVector3, Vector3> {
	using SoLib::Containers::WrappingClass<AlignasVector3, Vector3>::operator=;
	/// @brief メンバ変数ポインタを返す
	/// @return メンバ変数ポインタ
	inline static constexpr Vector3 AlignasVector3:: *ptr() noexcept { return &AlignasVector3::vec; }

	// ベクトルデータ
	Vector3 vec;
	// 意味のないデータ
	float pad;

	//__m128 &GetSimd() { return reinterpret_cast<__m128 &>(*this); }
};
