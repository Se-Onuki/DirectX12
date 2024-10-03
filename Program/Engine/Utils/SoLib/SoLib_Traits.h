#pragma once

#include <stdint.h>
#include <utility>

#include "../Math/Matrix2x2.h"
#include "../Math/Matrix3x3.h"
#include "../Math/Matrix4x4.h"
#include "../Math/Quaternion.h"
#include "../Math/Vector2.h"
#include "../Math/Vector3.h"
#include "../Math/Vector4.h"
#include <string>

namespace SoLib {

	/// @brief 非ポインタ型
	template<typename T>
	concept IsNotPointer = !std::is_pointer_v<T>;

	template <typename T>
	concept IsRealType = not std::is_pointer_v<T> and not std::is_reference_v<T>;

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

	/// @brief TがUを継承しているか
	template <typename T, typename U>
	concept IsBased = std::is_base_of_v<U, T>;

	template <typename T>
	concept IsConst = std::is_const_v<T>;

	/// @brief 浮動小数点型である
	template <typename T>
	concept IsFloatPoint = std::is_floating_point_v<T>;

	/// @brief 整数型である
	template <typename T>
	concept IsIntegral = std::is_integral_v<T>;

	/// @brief 数値型である
	template <typename T>
	concept IsNumber = std::is_floating_point_v<T> || std::is_integral_v<T>;

	template <typename Itr>
	concept IsIterator = requires(Itr iter) {
		// イテレータのデリファレンス可能性を確認
		{ *iter } -> std::same_as<typename std::iterator_traits<Itr>::value_type &>;

		// インクリメント可能性を確認
		{ ++iter } -> std::same_as<Itr &>;

		// インクリメント後のイテレータを作成できることを確認
		{ iter++ } -> std::same_as<Itr>;

		// 2つのイテレータを比較できることを確認
		{ iter == iter } -> std::same_as<bool>;

		// 2つのイテレータを比較できることを確認
		{ iter != iter } -> std::same_as<bool>;
	};

	template <typename Func, typename ReturnType, typename... Args>
	concept IsFunction = requires(Func func, Args... args) {
		// 関数の戻り値の型を確認
		{ func(args...) } -> std::same_as<ReturnType>;
	};

	template <typename R, typename T/*, T R:: *Ptr = nullptr*/>
	struct Wrapping {

		using WrappedType = T;

		inline operator T &() { return static_cast<R *>(this)->Get(); }
		inline operator const T &() const { return static_cast<const R *>(this)->Get(); }

		inline T &operator *() { return static_cast<R *>(this)->Get(); }
		inline const T &operator *() const { return static_cast<const R *>(this)->Get(); }

		Wrapping &operator=(const Wrapping &) = default;
		Wrapping &operator=(Wrapping &&) = default;

		R &operator=(const T &other) { static_cast<R *>(this)->Get() = other; return *static_cast<R *>(this); }
		R &operator=(T &&other) { static_cast<R *>(this)->Get() = std::move(other); return *static_cast<R *>(this); }

		bool operator ==(const T &other) { return static_cast<R *>(this)->Get() == other; }

		T &Get() {
			auto &ptr = *reinterpret_cast<T *>(this);
			return ptr;
			//return reinterpret_cast<R *>(this)->*(R::GetPtr());
		}

		const T &Get() const {
			return *reinterpret_cast<const T *>(this);
			//return reinterpret_cast<const R *>(this)->*(R::GetPtr());
		}

		//static T R:: *GetPtr() { return reinterpret_cast<T R:: *>(this); }

	};

	template <typename T>
	struct ContainerBeginEnd {
	private:
		T &Self() { return *reinterpret_cast<T *>(this); }
		const T &Self() const { return *reinterpret_cast<const T *>(this); }

	public:
		/// @brief 先頭要素のイテレータ
		/// @return イテレータ
		auto begin() { return Self().beginImpl(); }
		auto begin() const { return Self().beginImpl(); }

		/// @brief 先頭要素のイテレータ
		/// @return イテレータ
		auto cbegin() const { return Self().beginImpl(); }

		/// @brief 終端要素のイテレータ
		/// @return イテレータ
		auto end() { return Self().endImpl(); }
		auto end() const { return Self().endImpl(); }

		/// @brief 終端要素のイテレータ
		/// @return イテレータ
		auto cend() const { return Self().endImpl(); }
	};

	// メタ関数: 与えられた型のアライメントサイズを返す
	template<typename T>
	constexpr std::size_t Alignment = alignof(T);

	// メタ関数: 型リストから特定のアライメントサイズを持つ型を抽出する
	template<std::size_t Align, typename... Types>
	struct ExtractByAlignment;

	template<std::size_t Align>
	struct ExtractByAlignment<Align> {
		using Type = std::tuple<>;
	};

	template<std::size_t Align, typename T, typename... Rest>
	struct ExtractByAlignment<Align, T, Rest...> {
		using Type = std::conditional_t <
			(Alignment<T> == Align),
			decltype(std::tuple_cat(std::tuple<T>{}, typename ExtractByAlignment<Align, Rest...>::Type{})),
			typename ExtractByAlignment<Align, Rest...>::Type
		> ;
	};

	// メタ関数: 与えられた型リストを特定のアライメントサイズでソートする
	template<typename Tuple, std::size_t... Alignments>
	struct SortByAlignment;

	template<typename... Types, std::size_t... Alignments>
	struct SortByAlignment<std::tuple<Types...>, Alignments...> {
		using Type = decltype(std::tuple_cat(
			typename ExtractByAlignment<Alignments, Types...>::Type{}...
		));
	};

	// メタ関数を使いやすくするためのエイリアス
	template<typename Tuple>
	using SortByAlignment64 = SortByAlignment<Tuple, 64, 32, 16, 8, 4, 2, 1>;

	// ヘルパー関数テンプレート
	template<std::size_t... Is>
	auto ForEachImpl(std::index_sequence<Is...>) {
		return [](auto &&func, auto&&... args) {
			(func(std::integral_constant<std::size_t, Is>{}, args...), ...);
			};
	}

	// 型のリストを処理するForEach関数
	template<typename... Ts, typename Func>
	void ForEach(Func &&func, std::tuple<Ts...> tpl) {
		ForEachImpl < std::index_sequence_for<Ts...>{} > (std::forward<Func>(func), std::get<Ts>(tpl)...);
	}

	// 型Tがタプルの何番目の要素かを取得するメタ関数
	template<typename T, typename Tuple>
	struct IndexOf;

	// ベースケース：型Tが見つからなかった場合
	template<typename T>
	struct IndexOf<T, std::tuple<>> {
		static constexpr std::size_t value = -1; // 見つからなかった場合は-1
	};

	// 再帰ケース：最初の要素が型Tである場合
	template<typename T, typename... Rest>
	struct IndexOf<T, std::tuple<T, Rest...>> {
		static constexpr std::size_t value = 0;
	};

	// 再帰ケース：最初の要素が型Tでない場合
	template<typename T, typename U, typename... Rest>
	struct IndexOf<T, std::tuple<U, Rest...>> {
		static constexpr std::size_t value = 1 + IndexOf<T, std::tuple<Rest...>>::value;
	};

	// 与えられた型Tがタプルの何番目の要素かを取得する関数
	template<typename T, typename Tuple>
	constexpr std::size_t IndexOfV = IndexOf<T, Tuple>::value;

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