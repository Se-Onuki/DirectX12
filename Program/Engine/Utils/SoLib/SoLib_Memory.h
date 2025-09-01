/// @file SoLib_Memory.h
/// @brief メモリ関連のクラス
/// @auther ONUKI seiya
#pragma once
#include "SoLib_Traits.h"
#include <span>

namespace SoLib::Memory {

	template<IsRealType T>
	class view_pointer final {
	public:

		explicit view_pointer(T *pointer = nullptr) : pointer_(pointer) {}
		view_pointer(const view_pointer &pointer) : pointer_(pointer.pointer_) {}
		view_pointer operator=(const view_pointer &pointer) { pointer_ = pointer_; }

		bool operator==(const view_pointer &that) const { return this->pointer_ == that.pointer_; }

		bool operator==(const T *const ptr) const { return this->pointer_ == ptr; }

		explicit operator bool() const { return pointer_ != nullptr; }

		explicit operator T *() { return pointer_; }
		explicit operator const T *() const { return pointer_; }

		T *operator*() { return pointer_; }
		const T *operator*() const { return pointer_; }

		T *operator->() { return pointer_; }
		const T *operator->() const { return pointer_; }

		T *get() { return pointer_; }
		const T *get() const { return pointer_; }

	private:
		T *pointer_;
	};

	template<IsRealType T>
	class unique_array : public ContainerBeginEnd<unique_array<T>> {
	public:

		unique_array() = default;
		static unique_array &&make_unique(size_t size);
		unique_array(const unique_array &) = delete;
		unique_array &operator=(const unique_array &) = delete;
		unique_array(unique_array &&that) : buffer_(std::move(that.buffer_)), size_(that.size_) { that.reset(); }
		unique_array &&operator=(unique_array &&that) {
			reset();	// 現在持ってるデータを破棄
			buffer_ = std::move(that.buffer_);	// データの移譲
			size_ = that.size_;							// サイズのコピー
			that.reset();	// コピー元のデータを破棄
		}

		~unique_array() { reset(); }

		using iterator = T *;
		using const_iterator = const T *;

		iterator beginImpl() { return buffer_.get(); }
		const_iterator beginImpl() const { return buffer_.get(); }
		iterator endImpl() { return buffer_.get() + size_; }
		const_iterator endImpl() const { return buffer_.get() + size_; }

		size_t size() const { return size_; }

		explicit operator bool() const { return buffer_ && size_; }

		T &operator[](const size_t i) { return buffer_[i]; }
		const T &operator[](const size_t i) const { return buffer_[i]; }

		T &at(const size_t i) { return buffer_[i]; }
		const T &at(const size_t i) const { return buffer_[i]; }

		const std::span<T> operator*() { return { buffer_.get(), size_ }; }
		const std::span<const T> operator*() const { return { buffer_.get(), size_ }; }

		const std::span<T> get() { return { buffer_.get(), size_ }; }
		const std::span<const T> get() const { return { buffer_.get(), size_ }; }

		void reset();


	private:
		std::unique_ptr<T[]> buffer_;
		size_t size_;
	};

	template <IsRealType T>
	inline void unique_array<T>::reset() {
		size_ = 0;
		buffer_.reset();
	}

	template<IsRealType T>
	inline unique_array<T> &&unique_array<T>::make_unique(size_t size)
	{
		unique_array<T> result{};
		result.buffer_ = std::make_unique<T[]>(size);
		result.size_ = size;
		return std::move(result);
	}

}

namespace SoLib {
	template<SoLib::IsRealType T>
	using view_pointer = SoLib::Memory::view_pointer<T>;
}