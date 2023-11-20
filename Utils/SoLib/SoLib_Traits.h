#pragma once

#include <stdint.h>
#include <utility>

#include "../Math/Vector2.h"
#include "../Math/Vector3.h"
#include "../Math/Vector4.h"
#include "../Math/Matrix2x2.h"
#include "../Math/Matrix3x3.h"
#include "../Math/Matrix4x4.h"
#include <string>
#include "../Math/Quaternion.h"

namespace SoLib {

	/// @brief 非ポインタ型
	template<typename T>
	concept IsNotPointer = !std::is_pointer_v<T>;


	template<typename T>
	concept IsContainer = requires(T a) {
		{ a.size() } -> std::convertible_to<std::size_t>;
		{ a.begin() } -> std::input_or_output_iterator;
		{ a.end() } -> std::input_or_output_iterator;
	};

	template <typename T, typename U>
	concept IsContainsType = requires(T a) {

		requires IsContainer<T>; // Tがコンテナであることを確認
		requires std::same_as<typename T::value_type, U>; // Tの要素の型がUであることを確認
	};



	/// @brief 浮動小数点型である
	template <typename T>
	concept IsFloatPoint = std::is_floating_point_v<T>;

	/// @brief 整数型である
	template <typename T>
	concept IsIntegral = std::is_integral_v<T>;

	/// @brief 数値型である
	template <typename T>
	concept IsNumber = std::is_floating_point_v<T> || std::is_integral_v<T>;



	template <typename T>
	struct Traits {
		using Type = void;
		static constexpr auto Name = "NONE";
		static constexpr uint32_t Rows = 0u;
		static constexpr uint32_t Columns = 0u;
		static constexpr uint32_t Size = Rows * Columns;
		using ElementType = void;

		inline operator Traits<void> &() { return *this; }
	};

	template <>
	struct Traits<void> {
		using Type = void;
		static constexpr auto Name = "void";
		static constexpr uint32_t Rows = 0u;
		static constexpr uint32_t Columns = 0u;
		static constexpr uint32_t Size = Rows * Columns;
		using ElementType = void;
	};

	template<>
	struct Traits<int32_t> {
		using Type = int32_t;
		static constexpr auto Name = "int32_t";
		static constexpr uint32_t Rows = 1u;
		static constexpr uint32_t Columns = 1u;
		static constexpr uint32_t Size = Rows * Columns;
		using ElementType = int32_t;

		static const ElementType *CBegin(const Type &data) { return &data; }
		static const ElementType *End(const Type &data) { return CBegin(data) + Size; }
	};

	template<>
	struct Traits<float> {
		using Type = float;
		static constexpr auto Name = "float";
		static constexpr uint32_t Rows = 1u;
		static constexpr uint32_t Columns = 1u;
		static constexpr uint32_t Size = Rows * Columns;
		using ElementType = float;

		static const ElementType *CBegin(const Type &data) { return &data; }
		static const ElementType *End(const Type &data) { return CBegin(data) + Size; }
	};

	template<>
	struct Traits<Vector2> {
		using Type = Vector2;
		static constexpr auto Name = "Vector2";
		static constexpr uint32_t Rows = 1u;
		static constexpr uint32_t Columns = 2u;
		static constexpr uint32_t Size = Rows * Columns;
		using ElementType = float;

		static const ElementType *CBegin(const Type &data) { return data.cbegin(); }
		static const ElementType *End(const Type &data) { return CBegin(data) + Size; }
	};

	template<>
	struct Traits<Vector3> {
		using Type = Vector3;
		static constexpr auto Name = "Vector3";
		static constexpr uint32_t Rows = 1u;
		static constexpr uint32_t Columns = 3u;
		static constexpr uint32_t Size = Rows * Columns;
		using ElementType = float;

		static const ElementType *CBegin(const Type &data) { return data.cbegin(); }
		static const ElementType *End(const Type &data) { return CBegin(data) + Size; }
	};

	template<>
	struct Traits<Vector4> {
		using Type = Vector4;
		static constexpr auto Name = "Vector4";
		static constexpr uint32_t Rows = 1u;
		static constexpr uint32_t Columns = 4u;
		static constexpr uint32_t Size = Rows * Columns;
		using ElementType = float;

		static const ElementType *CBegin(const Type &data) { return data.cbegin(); }
		static const ElementType *End(const Type &data) { return CBegin(data) + Size; }
	};

	template<>
	struct Traits<Quaternion> {
		using Type = Quaternion;
		static constexpr auto Name = "Quaternion";
		static constexpr uint32_t Rows = 1u;
		static constexpr uint32_t Columns = 4u;
		static constexpr uint32_t Size = Rows * Columns;
		using ElementType = float;

		static const ElementType *CBegin(const Type &data) { return &data.x; }
		static const ElementType *End(const Type &data) { return CBegin(data) + Size; }
	};

	template<>
	struct Traits<Matrix2x2> {
		using Type = Matrix2x2;
		static constexpr auto Name = "Matrix2x2";
		static constexpr uint32_t Rows = 2u;
		static constexpr uint32_t Columns = 2u;
		static constexpr uint32_t Size = Rows * Columns;
		using ElementType = float;

		static const ElementType *CBegin(const Type &data) { return data.cbegin(); }
		static const ElementType *End(const Type &data) { return CBegin(data) + Size; }
	};

	template<>
	struct Traits<Matrix3x3> {
		using Type = Matrix3x3;
		static constexpr auto Name = "Matrix3x3";
		static constexpr uint32_t Rows = 3u;
		static constexpr uint32_t Columns = 3u;
		static constexpr uint32_t Size = Rows * Columns;
		using ElementType = float;

		static const ElementType *CBegin(const Type &data) { return data.cbegin(); }
		static const ElementType *End(const Type &data) { return CBegin(data) + Size; }
	};

	template<>
	struct Traits<Matrix4x4> {
		using Type = Matrix4x4;
		static constexpr auto Name = "Matrix4x4";
		static constexpr uint32_t Rows = 4u;
		static constexpr uint32_t Columns = 4u;
		static constexpr uint32_t Size = Rows * Columns;
		using ElementType = float;

		static const ElementType *CBegin(const Type &data) { return data.cbegin(); }
		static const ElementType *End(const Type &data) { return CBegin(data) + Size; }
	};


	template <typename T, typename = void>
	struct has_to_string : std::false_type {};

	template <typename T>
	struct has_to_string<T, std::void_t<decltype(std::to_string(std::declval<T>()))>> : std::true_type {};

	template <typename T>
	std::string to_string(const T &data) {
		if constexpr (has_to_string<T>::value) {
			return std::to_string(data);
		}
		else if constexpr (Traits<T>::Size == 0u) {
			return "";
		}
		else {

			std::string result;

			if constexpr (Traits<T>::Rows > 1u) {
				result += "{\n";
			}
			for (uint32_t y = 0; y < Traits<T>::Rows; ++y) {
				if constexpr (Traits<T>::Rows > 1u) {
					result += "\t";
				}
				if constexpr (Traits<T>::Columns > 1u) {
					result += "{ ";
				}
				for (uint32_t x = 0u; x < Traits<T>::Columns; ++x) {
					result += to_string(*(Traits<T>::CBegin(data) + y * Traits<T>::Columns + x));
					if (x < Traits<T>::Columns - 1u) {
						result += ", ";
					}
				}
				if constexpr (Traits<T>::Columns > 1u) {
					result += " }";
				}
				if (y < Traits<T>::Rows - 1u) {
					result += ",\n";
				}
			}

			if constexpr (Traits<T>::Rows > 1u) {
				result += "\n}";
			}

			return result;
		}
	}

	template<>
	inline std::string to_string<Quaternion>(const Quaternion &data) {
		return
			"{\n"
			"\tv : " + std::to_string(data.x) + ", " + std::to_string(data.y) + ", " + std::to_string(data.z) + ",\n"
			"\tw : " + std::to_string(data.w) + "\n"
			"}";
	}


}