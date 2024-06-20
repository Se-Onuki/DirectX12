#pragma once
#include "../SoLib/SoLib_Traits.h"
#include <cstdint>
#include <vector>
#include <algorithm>

namespace SoLib {

	template <SoLib::IsRealType T, uint32_t MaxSize>
	class ConstVector : public ContainerBeginEnd<ConstVector<T, MaxSize>> {
	public:
		using iterator = T *;
		using const_iterator = const T *;

		// 外部に表示される型
		using ArrayType = std::array<T, MaxSize>;
		// 内部的に管理する型
		using InnerArrayType = std::array<char, sizeof(T) *MaxSize>;

		/// @brief デストラクタを持っているか
		static constexpr bool kIsHasDestructor = !std::is_trivially_destructible_v<T>;

		ConstVector(uint32_t size = 0u) : size_(size) {}

		// デストラクタ
		~ConstVector() requires kIsHasDestructor { clear(); }	// デストラクタがある場合は実行してから破棄する
		~ConstVector() requires not kIsHasDestructor = default;	// デストラクタが無い場合はそのまま破棄する

		template <SoLib::IsContainsType<T> U>
		ConstVector(const U &cont) { *this = cont; }

		ConstVector(ConstVector &&move) :itemData_(move.itemData_), size_(move.size_) {}
		ConstVector &operator=(ConstVector &&move) { itemData_ = move.itemData_; size_ = move.size_; return *this; }

		template <SoLib::IsContainsType<T> U>
		inline ConstVector &operator=(const U &container) {
			uint32_t copyCount = (std::min)(MaxSize, static_cast<uint32_t>(container.size()));
			std::copy_n(container.begin(), copyCount, this->begin());

			size_ = copyCount;

			return *this;
		}

		template <SoLib::IsRealType U, uint32_t ThatSize>
			requires(std::same_as<T, U>)
		bool operator==(const ConstVector<U, ThatSize> &that) const {
			// 保存されている量が異なったら違う
			if (this->size() != that.size()) {
				return false;
			}
			//const std::span<T> aArr = std::span<T>{ this->GetArray().data(), this->size_};
			//const std::span<T> bArr = std::span<T>{ that.GetArray().data(), that.size_};

			//return aArr == bArr;

			// どれか一つでも違ったら異なる
			for (uint32_t i = 0; i < size(); i++) {
				if ((*this)[i] != that[i]) {
					return false;
				}
			}

			return true;
		}

		T &operator[](const uint32_t index) { return GetArray()[index]; }
		const T &operator[](const uint32_t index) const { return GetArray()[index]; }

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
			return size_ == MaxSize;
		}

		void push_back(const T &other);
		void push_back(T &&other);

		T &at(uint32_t index) { return GetArray()[index]; }
		const T &at(uint32_t index) const { return GetArray()[index]; }

	private:

		friend ContainerBeginEnd<ConstVector<T, MaxSize>>;

		iterator beginImpl() { return GetArray().data(); }
		const_iterator beginImpl() const { return GetArray().data(); }

		iterator endImpl() { return GetArray().data() + size_; }
		const_iterator endImpl() const { return GetArray().data() + size_; }

	private:

		ArrayType &GetArray() { return *reinterpret_cast<ArrayType *>(itemData_.data()); }
		const ArrayType &GetArray() const { return *reinterpret_cast<const ArrayType *>(itemData_.data()); }
		// 配列
		alignas(alignof(T)) InnerArrayType itemData_ {};
		uint32_t size_;
	};

	template <SoLib::IsRealType T, uint32_t MaxSize>
	inline bool ConstVector<T, MaxSize>::ImGuiWidget(const char *const label)
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

	template<SoLib::IsRealType T, uint32_t MaxSize>
	inline void ConstVector<T, MaxSize>::push_back(const T &other) {
		if (MaxSize > size_) {
			// 対象となるアドレス
			T *itemPtr = &GetArray()[size_++];
			// 配置new
			new(itemPtr)T{ other };

		}
	}

	template<SoLib::IsRealType T, uint32_t MaxSize>
	inline void ConstVector<T, MaxSize>::push_back(T &&other) {
		if (MaxSize > size_) {

			// 対象となるアドレス
			T *itemPtr = &GetArray()[size_++];
			// 配置new
			new(itemPtr)T{ std::move(other) };

		}
	}

	template <SoLib::IsRealType T, uint32_t MaxSize>
	void from_json(const nlohmann::json &json, ConstVector<T, MaxSize> &value) {
		value = json.get<std::vector<T>>();
	}

}