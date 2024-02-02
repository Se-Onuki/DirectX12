#pragma once
#include "../SoLib/SoLib_Traits.h"
#include <cstdint>
#include <vector>

namespace SoLib {

	template <SoLib::IsNotPointer T, uint32_t maxSize>
	class ConstVector : public ContainerBeginEnd<ConstVector<T, maxSize>> {
	public:
		using iterator = T *;
		using const_iterator = const T *;

		using ConstArrayType = std::array<T, maxSize>;

		ConstVector(uint32_t size = 0u) : size_(size) {}

		template <SoLib::IsContainsType<T> U>
		ConstVector(const U &cont) { *this = cont; }

		template <SoLib::IsContainsType<T> U>
		inline ConstVector &operator=(const U &container) {
			uint32_t copyCount = (std::min)(maxSize, static_cast<uint32_t>(container.size()));
			std::copy_n(container.begin(), copyCount, this->begin());

			size_ = copyCount;

			return *this;
		}

		T &operator[](const uint32_t index) { itemData_[index]; }
		const T &operator[](const uint32_t index) const { itemData_[index]; }

		bool ImGuiWidget(const char *const label);

		uint32_t size() const { return size_; }

		void clear() {
			for (auto &item : itemData_) {
				std::destroy_at(&item);
			}
			size_ = 0u;
		}

		void push_back(const T &other);
		void push_back(T &&other);

		iterator beginImpl() { return itemData_; }
		const_iterator beginImpl() const { return itemData_; }

		iterator endImpl() { return itemData_ + size_; }
		const_iterator endImpl() const { return itemData_ + size_; }

		/*T *const data() { return itemData_.data(); }
		const T *const data() const { return itemData_.data(); }

		ConstArrayType::iterator begin() { return itemData_.begin(); }
		ConstArrayType::const_iterator begin() const { return itemData_.begin(); }

		ConstArrayType::iterator end() { return itemData_.begin() + size_; }
		ConstArrayType::const_iterator end() const { return itemData_.begin() + size_; }*/

	private:
		// 配列
		std::array<T, maxSize> itemData_;
		uint32_t size_;
	};

	template <SoLib::IsNotPointer T, uint32_t maxSize>
	inline bool ConstVector<T, maxSize>::ImGuiWidget(const char *const label)
	{
#ifdef _DEBUG

		bool isChanged = false;

		for (uint32_t i = 0; i < size_; i++) {
			isChanged |= SoLib::ImGuiWidget((label + std::to_string(i)).c_str(), &itemData_[i]);
		}

		return isChanged;

#else
		label;
		return false;

#endif // _DEBUG
	}

	template<SoLib::IsNotPointer T, uint32_t maxSize>
	inline void ConstVector<T, maxSize>::push_back(const T &other) {
		if (maxSize > size_) {

			itemData_[size_++] = other;

		}
	}

	template<SoLib::IsNotPointer T, uint32_t maxSize>
	inline void ConstVector<T, maxSize>::push_back(T &&other) {
		if (maxSize > size_) {

			itemData_[size_++] = std::move(other);

		}
	}

	template <SoLib::IsNotPointer T, uint32_t maxSize>
	void from_json(const nlohmann::json &json, ConstVector<T, maxSize> &value) {
		value = json.get<std::vector<T>>();
	}

}