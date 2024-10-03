#pragma once
#include <span>
#include <cstdint>
#include <vector>
#include <memory>

#include "ComponentArray.h"

namespace ECS {

	class ComponentData;

	template<typename T, bool IsConst>
	class CompIterator {
	public:

		inline static constexpr bool kIsByte = std::is_same_v<T, std::byte>;

		template<typename U>
		using ConstType = std::conditional_t<IsConst, const U, U>;

		using DataType = std::conditional_t<IsConst, const ComponentData *, ComponentData *>;

		using difference_type = int32_t;
		using value_type = ConstType<T>;
		using iterator_category = std::random_access_iterator_tag;

		auto operator*()->value_type &;
		auto operator[](uint32_t index) const->value_type &;

		DataType target_;

		uint32_t index_;

	};

	class ComponentData {
	public:

		class Range {
		public:
			using iterator = CompIterator<std::byte, false>;
			using const_iterator = CompIterator<std::byte, true>;

			Range() = default;
			Range(const Range &) = default;
			Range(Range &&) = default;
			Range &operator=(const Range &) = default;
			Range &operator=(Range &&) = default;
			Range(ComponentData *compData, uint32_t begin, uint32_t end) : compData_(compData), begin_(begin), end_(end) {}

			iterator begin() { return iterator{ compData_, begin_ }; }
			const_iterator begin() const { return const_iterator{ compData_, begin_ }; }
			const_iterator cbegin() const { return const_iterator{ compData_, begin_ }; }

			iterator end() { return  iterator{ compData_, end_ }; }
			const_iterator end() const { return const_iterator{ compData_, end_ }; }
			const_iterator cend() const { return const_iterator{ compData_, end_ }; }

			uint32_t size() const { return end_ - begin_; }

		private:

			ComponentData *compData_;
			uint32_t begin_;
			uint32_t end_;

		};

		template<typename T, bool IsConst = false>
		class TRange {
		public:

			using iterator = CompIterator<T, false>;
			using const_iterator = CompIterator<T, true>;

			using CompData = std::conditional_t<IsConst, const ComponentData *, ComponentData *>;

			TRange() = default;
			TRange(const TRange &) = default;
			TRange(TRange &&) = default;
			TRange &operator=(const TRange &) = default;
			TRange &operator=(TRange &&) = default;
			TRange(CompData compData, uint32_t begin, uint32_t end) : compData_(compData), begin_(begin), end_(end) {}

			iterator begin() requires(IsConst == false) { return iterator{ compData_, begin_ }; }
			const_iterator begin() const { return const_iterator{ compData_, begin_ }; }
			const_iterator cbegin() const { return const_iterator{ compData_, begin_ }; }

			iterator end() requires(IsConst == false) { return iterator{ compData_, end_ }; }
			const_iterator end() const { return const_iterator{ compData_, end_ }; }
			const_iterator cend() const { return const_iterator{ compData_, end_ }; }

			T &operator[](uint32_t i) requires(IsConst == false) { return compData_->at<T>(begin_ + i); }
			const T &operator[](uint32_t i) const { return compData_->at<T>(begin_ + i); }

			uint32_t size() const { return end_ - begin_; }

		private:

			friend TRange<T, true>;
			friend TRange<T, false>;

			CompData compData_;
			uint32_t begin_;
			uint32_t end_;

		};

	public:
		ComponentData() = default;
		ComponentData(const ComponentData &) = default;
		ComponentData &operator=(const ComponentData &) = default;
		ComponentData(uint32_t itemSize, uint32_t count) : typeSize_(itemSize), itemCount_(count) {}

		void AddArray(std::byte *ptr) { components_.push_back({ ptr,typeSize_, itemCount_ }); }

		std::span<ComponentArray> GetCompArray() { return { components_.data(), components_.size() }; }

		std::byte *operator[](uint32_t index) { const uint32_t groupSize = typeSize_ * itemCount_; return components_[index / groupSize][index % groupSize]; }
		const std::byte *operator[](uint32_t index) const { const uint32_t groupSize = typeSize_ * itemCount_; return components_[index / groupSize][index % groupSize]; }

		std::byte &at(uint32_t index) { const uint32_t groupSize = typeSize_ * itemCount_; return *components_[index / groupSize][index % groupSize]; }
		const std::byte &at(uint32_t index) const { const uint32_t groupSize = typeSize_ * itemCount_; return *components_[index / groupSize][index % groupSize]; }

		template<typename T>
		T &at(uint32_t index) { return reinterpret_cast<T &>(at(index)); }
		template<typename T>
		const T &at(uint32_t index) const { return reinterpret_cast<const T &>(at(index)); }

		Range View(uint32_t end) { return Range{ this,0,end }; }
		Range View(uint32_t begin, uint32_t end) { return Range{ this,begin,end }; }

		template<typename T>
		TRange<T> View(uint32_t end) { return TRange<T>{ this, 0u, end }; }
		template<typename T>
		TRange<T> View(uint32_t begin, uint32_t end) { return TRange<T>{ this, begin, end }; }

		template<typename T>
		TRange<T, true> View(uint32_t end) const { return TRange<T, true>{ this, 0u, end }; }
		template<typename T>
		TRange<T, true> View(uint32_t begin, uint32_t end) const { return TRange<T, true>{ this, begin, end }; }

		uint32_t GetTypeSize() const { return typeSize_; }


	private:
		// コンポーネントの配列
		std::vector<ComponentArray> components_;
		// 形のサイズ
		uint32_t typeSize_;
		// コンポーネント配列の長さ
		uint32_t itemCount_;
	};

#pragma region Func

	template<typename T, bool IsConst>
	auto operator++(CompIterator<T, IsConst> &itr) -> CompIterator<T, IsConst> &
	{
		itr.index_++;
		return itr;
	}

	template<typename T, bool IsConst>
	auto operator++(CompIterator<T, IsConst> &itr, int) -> CompIterator<T, IsConst>
	{
		auto result = itr;
		itr.index_++;
		return result;
	}

	template<typename T, bool IsConst>
	auto operator--(CompIterator<T, IsConst> &itr) -> CompIterator<T, IsConst> &
	{
		itr.index_--;
		return itr;
	}

	template<typename T, bool IsConst>
	auto operator--(CompIterator<T, IsConst> &itr, int) -> CompIterator<T, IsConst>
	{
		auto result = itr;
		itr.index_--;
		return result;
	}

	template<typename T, bool IsConst>
	auto operator+(const CompIterator<T, IsConst> &itr, int32_t diff) -> CompIterator<T, IsConst>
	{
		CompIterator<T, IsConst> result = itr;
		result.index_ += diff;
		return result;
	}
	template<typename T, bool IsConst>
	auto operator+(int32_t diff, const CompIterator<T, IsConst> &itr) -> CompIterator<T, IsConst>
	{
		CompIterator<T, IsConst> result = itr;
		result.index_ += diff;
		return result;
	}
	template<typename T, bool IsConst>
	auto operator+=(CompIterator<T, IsConst> &itr, int32_t diff) -> CompIterator<T, IsConst> &
	{
		itr.index_ += diff;
		return itr;
	}
	template<typename T, bool IsConst>
	auto operator-(const CompIterator<T, IsConst> &itr, int32_t diff) -> CompIterator<T, IsConst>
	{
		CompIterator<T, IsConst> result = itr;
		result.index_ -= diff;
		return result;
	}
	template<typename T, bool IsConst>
	auto operator-=(CompIterator<T, IsConst> &itr, int32_t diff) -> CompIterator<T, IsConst> &
	{
		itr.index_ -= diff;
		return itr;
	}
	template<typename T, bool IsConst>
	auto operator-(const CompIterator<T, IsConst> &l, const CompIterator<T, IsConst> &r) -> CompIterator<T, IsConst>::difference_type
	{
		return l.index_ - r.index_;
	}

	template<typename T, bool IsConst>
	auto CompIterator<T, IsConst>::operator*() -> CompIterator<T, IsConst>::value_type &
	{
		if constexpr (kIsByte) {
			return *target_->at(index_);
		}
		else {
			return target_->at<T>(index_);
		}
	}
	template<typename T, bool IsConst>
	bool operator==(const CompIterator<T, IsConst> &l, const CompIterator<T, IsConst> &r)
	{
		return l.index_ == r.index_ and l.target_ == r.target_;
	}
	template<typename T, bool IsConst>
	std::strong_ordering operator<=>(const CompIterator<T, IsConst> &l, const CompIterator<T, IsConst> &r)
	{
		if (auto comp = l.index_ <=> r.index_; comp != 0) return comp;
		return l.target_ <=> r.target_;
	}

	template<typename T, bool IsConst>
	auto CompIterator<T, IsConst>::operator[](uint32_t index_) const -> CompIterator<T, IsConst>::value_type &
	{
		if constexpr (kIsByte) {
			return *target_->at(index_);
		}
		else {
			return target_->at<T>(index_);
		}
	}

#pragma endregion

}
