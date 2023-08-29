#pragma once

#include <stdint.h>
#include <string>

#include "../../Header/Math/Vector2.h"
#include "../../Header/Math/Vector3.h"
#include "../../Header/Math/Vector4.h"

#include "../../Header/Math/Matrix2x2.h"
#include "../../Header/Math/Matrix3x3.h"
#include "../../Header/Math/Matrix4x4.h"

#include "../../Header/File/VariantItem.h"

namespace SoLib {

	void ImGuiWidget(const char *const label, int32_t *const value);
	void ImGuiWidget(const char *const label, float *const value);
	void ImGuiWidget(const char *const label, double *const value);
	void ImGuiWidget(const char *const label, Vector2 *const value);
	void ImGuiWidget(const char *const label, Vector3 *const value);
	void ImGuiWidget(const char *const label, Vector4 *const value);

	template<typename T>
	void ImGuiWidget(VariantItem<T> *const value) { ImGuiWidget(value->GetKey().c_str(), &value->GetItem()); }

}