#pragma once
#include "../SoLib/SoLib_Traits.h"
#include <cstdint>
#include <vector>
#include <algorithm>

namespace SoLib {

	template <SoLib::IsNotPointer T, uint32_t maxSize>
	class ConstVector : public ContainerBeginEnd<ConstVector<T, maxSize>> {
	public:
		using iterator = T *;
		using const_iterator = const T *;

		using ArrayType = std::array<T, maxSize>;
		using InnerArrayType = std::array<char, sizeof(T) *maxSize>;

		/// @brief デストラクタを持っているか
		static constexpr bool kIsHasDestructor = !std::is_trivially_destructible_v<T>;

		ConstVector(uint32_t size = 0u) : size_(size) {}

		// デストラクタ
		~ConstVector() requires kIsHasDestructor { clear(); }	// デストラクタがある場合は実行してから破棄する
		~ConstVector() requires not kIsHasDestructor = default;	// デストラクタが無い場合はそのまま破棄する

		template <SoLib::IsContainsType<T> U>
		ConstVector(const U &cont) { *this = cont; }

		template <SoLib::IsContainsType<T> U>
		inline ConstVector &operator=(const U &container) {
			uint32_t copyCount = (std::min)(maxSize, static_cast<uint32_t>(container.size()));
			std::copy_n(container.begin(), copyCount, this->begin());

			size_ = copyCount;

			return *this;
		}

		T &operator[](const uint32_t index) { GetArray()[index]; }
		const T &operator[](const uint32_t index) const { GetArray()[index]; }

		bool ImGuiWidget(const char *const label);

		uint32_t size() const { return size_; }

		void clear() {
			if constexpr (kIsHasDestructor) {
				for (uint32_t i = 0u; i < size_; i++) {
					std::destroy_at(&GetArray()[i]);
				}
			}
			size_ = 0u;
		}

		bool IsMax()const {
			return size_ == maxSize;
		}

		void push_back(const T &other);
		void push_back(T &&other);

		iterator beginImpl() { return GetArray().data(); }
		const_iterator beginImpl() const { return GetArray().data(); }

		iterator endImpl() { return GetArray().data() + size_; }
		const_iterator endImpl() const { return GetArray().data() + size_; }

		T &at(uint32_t index) { return GetArray()[index]; }
		const T &at(uint32_t index) const { return GetArray()[index]; }


	private:

		ArrayType &GetArray() { return *reinterpret_cast<ArrayType *>(itemData_.data()); }
		const ArrayType &GetArray() const { return *reinterpret_cast<const ArrayType *>(itemData_.data()); }
		// 配列
		alignas(alignof(T)) InnerArrayType itemData_ {};
		uint32_t size_;
	};

	template <SoLib::IsNotPointer T, uint32_t maxSize>
	inline bool ConstVector<T, maxSize>::ImGuiWidget(const char *const label)
	{
#ifdef USE_IMGUI

		bool isChanged = false;

		for (uint32_t i = 0; i < size_; i++) {
			isChanged |= SoLib::ImGuiWidget((label + std::to_string(i)).c_str(), &GetArray()[i]);
		}

		return isChanged;

#else
		label;
		return false;

#endif // USE_IMGUI
	}

	template<SoLib::IsNotPointer T, uint32_t maxSize>
	inline void ConstVector<T, maxSize>::push_back(const T &other) {
		if (maxSize > size_) {
			// 対象となるアドレス
			T *itemPtr = &GetArray()[size_++];
			// 配置new
			new(itemPtr)T{ other };

		}
	}

	template<SoLib::IsNotPointer T, uint32_t maxSize>
	inline void ConstVector<T, maxSize>::push_back(T &&other) {
		if (maxSize > size_) {

			// 対象となるアドレス
			T *itemPtr = &GetArray()[size_++];
			// 配置new
			new(itemPtr)T{ std::move(other) };

		}
	}

	template <SoLib::IsNotPointer T, uint32_t maxSize>
	void from_json(const nlohmann::json &json, ConstVector<T, maxSize> &value) {
		value = json.get<std::vector<T>>();
	}

}