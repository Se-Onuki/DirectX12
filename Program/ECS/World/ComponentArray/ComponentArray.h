#pragma once
#include <span>
#include <cstdint>

namespace ECS {
	/// @brief 配列としてコンポーネントを扱う型
	class ComponentArray {
	public:
		ComponentArray() = default;
		ComponentArray(const ComponentArray &) = default;
		ComponentArray &operator=(const ComponentArray &) = default;
		ComponentArray(std::byte *ptr, uint32_t size, uint32_t count) : ptr_(ptr), size_(size), count_(count) {}

		template<typename T>
		bool IsContain() const { return sizeof(T) == size_; }

		/// @brief 配列の取得
		/// @return 配列の取得
		template<typename T>
		const std::span<T> GetArray() { return { reinterpret_cast<T *>(ptr_), count_ }; }
		template<typename T>
		const std::span<const T> GetArray() const { return { reinterpret_cast<const T *>(ptr_), count_ }; }

		const std::span<std::byte> GetArray() { return { ptr_, size_ * count_ }; }
		const std::span<const std::byte> GetArray() const { return { ptr_, size_ * count_ }; }

		std::byte *operator[](const uint32_t index) { return ptr_ + size_ * index; }
		const std::byte *operator[](const uint32_t index) const { return ptr_ + size_ * index; }

		std::byte *at(const uint32_t index) { return ptr_ + size_ * index; }
		const std::byte *at(const uint32_t index) const { return ptr_ + size_ * index; }

		std::byte *data() { return ptr_; }
		const std::byte *data() const { return ptr_; }
		std::byte *begin() { return ptr_; }
		const std::byte *begin() const { return ptr_; }
		std::byte *end() { return ptr_ + size_ * count_; }
		const std::byte *end() const { return ptr_ + size_ * count_; }

		uint32_t size() const { return count_; }

	private:
		// 配列の始点
		std::byte *ptr_;
		// 形のサイズ
		uint32_t size_;
		// 配列の数
		uint32_t count_;

	};
}