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

	bool ImGuiWidget(const char *const label, bool *const value);

	bool ImGuiWidget(const char *const label, int32_t *const value);
	bool ImGuiWidget(const char *const label, float *const value);
	bool ImGuiWidget(const char *const label, double *const value);
	bool ImGuiWidget(const char *const label, Vector2 *const value);
	bool ImGuiWidget(const char *const label, Vector3 *const value);
	bool ImGuiWidget(const char *const label, Vector4 *const value);

	bool ImGuiWidget(const char *const label, std::string *const value);

	template<typename T>
	bool ImGuiWidget(VariantItem<T> *const value) { return ImGuiWidget(value->GetKey().c_str(), &value->GetItem()); }

}