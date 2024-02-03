#pragma once
#include "Vector3.h"
#include "Matrix4x4.h"
#include "../SoLib/SoLib_Json.h"
#include "../SoLib/SoLib_ImGui.h"
#include <initializer_list>

namespace SoLib {
	namespace Math {

		struct Euler {

			Euler() = default;
			Euler(float x, float y, float z) :x(x), y(y), z(z) {}

			Euler(const Euler &) = default;
			Euler(Euler &&) = default;

			Euler &operator=(const Euler &) = default;
			Euler &operator=(Euler &&) = default;

			Euler(const Vector3 &other) { *this = *reinterpret_cast<const Euler *>(&other); }
			Euler(Vector3 &&other) { *this = Euler(other); }

			Euler &operator=(const Vector3 &other) { return *this = Euler(other); }
			Euler &operator=(Vector3 &&other) { return *this = Euler(other); }

			//Euler &operator=(std::initializer_list<float> &&other) {
			//	std::copy(other.begin(), other.end(), *this);
			//	return *this;
			//}

			inline operator Vector3 &() { return *reinterpret_cast<Vector3 *>(this); }
			inline operator const Vector3 &() const { return *reinterpret_cast<const Vector3 *>(this); }

			static uint32_t size() { return 3u; }

			float *const begin() { return &x; }
			const float *const begin() const { return &x; }

			float *const end() { return &x + 3u; }
			const float *const end() const { return &x + 3u; }

			float x, y, z;

		};

		inline void to_json(nlohmann::json &json, const Euler &value) {
			json = static_cast<const Vector3 &>(value);
		}

		inline void from_json(const nlohmann::json &json, Euler &value) {
			value = json.get<Vector3>();
		}

	}
}


template<>
inline bool SoLib::ImGuiWidget<SoLib::Math::Euler>(const char *const label, SoLib::Math::Euler *const value) {
#ifdef _DEBUG
	return SoLib::ImGuiDragEuler(label, &value->x);
#else
	label; value;
	return false;
#endif // _DEBUG
}