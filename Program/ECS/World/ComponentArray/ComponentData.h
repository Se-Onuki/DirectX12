#pragma once
#include <span>
#include <cstdint>
#include <vector>
#include <memory>

#include "ComponentArray.h"

namespace ECS {

	class ComponentData;

	template<bool IsConst>
	class CompIterator {
	public:

		template<typename T>
		using ConstType = std::conditional_t<IsConst, const T, T>;

		using DataType = std::conditional_t<IsConst, const ComponentData *, ComponentData *>;

		using difference_type = int32_t;
		using value_type = ConstType<std::byte>;
		using iterator_category = std::random_access_iterator_tag;

		friend auto operator++(CompIterator &)->CompIterator &;
		friend auto operator++(CompIterator &, int)->CompIterator;

		friend auto operator--(CompIterator &)->CompIterator &;
		friend auto operator--(CompIterator &, int)->CompIterator;

		friend auto operator+(const CompIterator &, int32_t)->CompIterator;
		friend auto operator+(int32_t, const CompIterator &)->CompIterator;
		friend auto operator+=(CompIterator &, int32_t)->CompIterator &;
		friend auto operator-(const CompIterator &, int32_t)->CompIterator;
		friend auto operator-=(CompIterator &, int32_t)->CompIterator &;

		friend auto operator-(const CompIterator &, const CompIterator &)->difference_type;

		friend auto operator*(const CompIterator &)->value_type &;
		auto operator[](uint32_t index) const->value_type &;

		friend std::strong_ordering operator<=>(const CompIterator &, const CompIterator &);
		friend bool operator==(const CompIterator &, const CompIterator &);

		DataType target_;

		uint32_t index_;

	};


	template <typename T, bool IsConst>
	struct TCompIterator : public CompIterator<IsConst> {

		template<typename U>
		using ConstType = std::conditional_t<IsConst, const U, U>;

		using value_type = ConstType<T>;

		auto operator*()->value_type &;
		auto operator[](uint32_t index) const->value_type &;
	};

	class ComponentData {
	public:

		class Range {
		public:
			using iterator = CompIterator<false>;
			using const_iterator = CompIterator<true>;

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

		template<typename T>
		class TRange {
		public:

			using iterator = TCompIterator<T, false>;
			using const_iterator = TCompIterator<T, true>;

			TRange() = default;
			TRange(const TRange &) = default;
			TRange(TRange &&) = default;
			TRange &operator=(const TRange &) = default;
			TRange &operator=(TRange &&) = default;
			TRange(ComponentData *compData, uint32_t begin, uint32_t end) : compData_(compData), begin_(begin), end_(end) {}

			iterator begin() { return iterator{ compData_, begin_ }; }
			const_iterator begin() const { return const_iterator{ compData_, begin_ }; }
			const_iterator cbegin() const { return const_iterator{ compData_, begin_ }; }

			iterator end() { return iterator{ compData_, end_ }; }
			const_iterator end() const { return const_iterator{ compData_, end_ }; }
			const_iterator cend() const { return const_iterator{ compData_, end_ }; }

			uint32_t size() const { return end_ - begin_; }

		private:

			ComponentData *compData_;
			uint32_t begin_;
			uint32_t end_;

		};

	public:
		ComponentData() = default;
		ComponentData(const ComponentData &) = default;
		ComponentData &operator=(const ComponentData &) = default;
		ComponentData(uint32_t itemSize, uint32_t count) : itemSize_(itemSize), itemCount_(count) {}

		void AddArray(std::byte *ptr) { components_.push_back({ ptr,itemSize_, itemCount_ }); }

		std::span<ComponentArray> GetCompArray() { return { components_.data(), components_.size() }; }

		std::byte *operator[](uint32_t index) { const uint32_t groupSize = itemSize_ * itemCount_; return components_[index / groupSize][index % groupSize]; }
		const std::byte *operator[](uint32_t index) const { const uint32_t groupSize = itemSize_ * itemCount_; return components_[index / groupSize][index % groupSize]; }

		std::byte &at(uint32_t index) { const uint32_t groupSize = itemSize_ * itemCount_; return *components_[index / groupSize][index % groupSize]; }
		const std::byte &at(uint32_t index) const { const uint32_t groupSize = itemSize_ * itemCount_; return *components_[index / groupSize][index % groupSize]; }

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


	private:
		// コンポーネントの配列
		std::vector<ComponentArray> components_;
		// 形のサイズ
		uint32_t itemSize_;
		// コンポーネント配列の長さ
		uint32_t itemCount_;
	};

#pragma region Func

	template<bool IsConst>
	auto operator++(CompIterator<IsConst> &itr) -> CompIterator<IsConst> &
	{
		itr.index_++;
		return itr;
	}

	template<bool IsConst>
	auto operator++(CompIterator<IsConst> &itr, int) -> CompIterator<IsConst>
	{
		auto result = itr;
		itr.index_++;
		return result;
	}

	template<bool IsConst>
	auto operator--(CompIterator<IsConst> &itr) -> CompIterator<IsConst> &
	{
		itr.index_--;
		return itr;
	}

	template<bool IsConst>
	auto operator--(CompIterator<IsConst> &itr, int) -> CompIterator<IsConst>
	{
		auto result = itr;
		itr.index_--;
		return result;
	}

	template<bool IsConst>
	auto operator+(const CompIterator<IsConst> &itr, int32_t diff) -> CompIterator<IsConst>
	{
		CompIterator<IsConst> result = itr;
		result.index_ += diff;
		return result;
	}
	template<bool IsConst>
	auto operator+(int32_t diff, const CompIterator<IsConst> &itr) -> CompIterator<IsConst>
	{
		CompIterator<IsConst> result = itr;
		result.index_ += diff;
		return result;
	}
	template<bool IsConst>
	auto operator+=(CompIterator<IsConst> &itr, int32_t diff) -> CompIterator<IsConst> &
	{
		itr.index_ += diff;
		return itr;
	}
	template<bool IsConst>
	auto operator-(const CompIterator<IsConst> &itr, int32_t diff) -> CompIterator<IsConst>
	{
		CompIterator<IsConst> result = itr;
		result.index_ -= diff;
		return result;
	}
	template<bool IsConst>
	auto operator-=(CompIterator<IsConst> &itr, int32_t diff) -> CompIterator<IsConst> &
	{
		itr.index_ -= diff;
		return itr;
	}
	template<bool IsConst>
	auto operator-(const CompIterator<IsConst> &l, const CompIterator<IsConst> &r) -> CompIterator<IsConst>::difference_type
	{
		return l.index_ - r.index_;
	}

	template<bool IsConst>
	auto operator*(const CompIterator<IsConst> &itr) -> CompIterator<IsConst>::value_type &
	{
		return itr.target_->at(itr.index_);
	}
	template<bool IsConst>
	bool operator==(const CompIterator<IsConst> &l, const CompIterator<IsConst> &r)
	{
		return l.index_ == r.index_ and l.target_ == r.target_;
	}
	template<bool IsConst>
	std::strong_ordering operator<=>(const CompIterator<IsConst> &l, const CompIterator<IsConst> &r)
	{
		if (auto comp = l.index_ <=> r.index_; comp != 0) return comp;
		return l.target_ <=> r.target_;
	}

	template<bool IsConst>
	auto CompIterator<IsConst>::operator[](uint32_t index_) const -> CompIterator<IsConst>::value_type &
	{
		return *target_->at(index_);
	}

#pragma region TemplateRange

	template<typename T, bool IsConst>
	auto TCompIterator<T, IsConst>::operator[](uint32_t index) const -> TCompIterator<T, IsConst>::value_type &
	{
		return *reinterpret_cast<value_type *>((*static_cast<const CompIterator<IsConst>*>(this))[index]);
	}

	template<typename T, bool IsConst>
	auto TCompIterator<T, IsConst>::operator*() -> typename TCompIterator<T, IsConst>::value_type &
	{
		return this->target_->at<T>(this->index_);
	}

#pragma endregion



#pragma endregion

}
