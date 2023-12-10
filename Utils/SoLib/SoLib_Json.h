#pragma once
#include "SoLib_Traits.h"
#include "../../externals/nlohmann/json.hpp"

#include "../Math/Matrix2x2.h"
#include "../Math/Matrix3x3.h"
#include "../Math/Matrix4x4.h"
#include "../Math/Quaternion.h"
#include "../Math/Vector2.h"
#include "../Math/Vector3.h"
#include "../Math/Vector4.h"

void from_json(const nlohmann::json &json, Vector2 &data);
void from_json(const nlohmann::json &json, Vector3 &data);
void from_json(const nlohmann::json &json, Vector4 &data);


void to_json(nlohmann::json &json, const  Quaternion &data);
void from_json(const nlohmann::json &json, Quaternion &data);
