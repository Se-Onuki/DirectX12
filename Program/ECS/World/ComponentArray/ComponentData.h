#pragma once
#include <span>
#include <cstdint>
#include <vector>
#include <memory>

#include "ComponentArray.h"

namespace ECS {

	template<bool IsConst>
	class CompIterator;

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

			iterator begin() { iterator{ compData_, begin_ }; }
			const_iterator begin() const { const_iterator{ compData_, begin_ }; }
			const_iterator cbegin() const { const_iterator{ compData_, begin_ }; }

			iterator end() { iterator{ compData_, end_ }; }
			const_iterator end() const { const_iterator{ compData_, end_ }; }
			const_iterator cend() const { const_iterator{ compData_, end_ }; }

			uint32_t size() const { return end_ - begin_; }

		private:

			ComponentData *compData_;
			uint32_t begin_;
			uint32_t end_;

		};


	public:



		std::span<ComponentArray> GetCompArray() { return { components_.data(), components_.size() }; }

		std::byte *operator[](uint32_t index) { const uint32_t itemSize_ = size_ * count_; return components_[index / itemSize_][index % itemSize_]; }
		const std::byte *operator[](uint32_t index) const { const uint32_t itemSize_ = size_ * count_; return components_[index / itemSize_][index % itemSize_]; }

		std::byte *at(uint32_t index) { const uint32_t itemSize_ = size_ * count_; return components_[index / itemSize_][index % itemSize_]; }
		const std::byte *at(uint32_t index) const { const uint32_t itemSize_ = size_ * count_; return components_[index / itemSize_][index % itemSize_]; }

		uint32_t size() const { return components_.empty() ? 0u : count_ * (components_.size() - 1) + components_.back().size(); }

		Range View(uint32_t end) { return Range{ this,0,end }; }
		Range View(uint32_t begin, uint32_t end) { return Range{ this,begin,end }; }


	private:
		// コンポーネントの配列
		std::vector<ComponentArray> components_;
		// 形のサイズ
		uint32_t size_;
		// 配列の数
		uint32_t count_;
	};

	template<bool IsConst>
	class CompIterator {
	public:

		using difference_type = int32_t;
		using value_type = std::byte *;
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
		auto operator[](int) const->value_type &;

		friend std::strong_ordering operator<=>(const CompIterator &, const CompIterator &);
		friend bool operator==(const CompIterator &, const CompIterator &);

		ComponentData *target_;

		uint32_t index_;

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
	bool operator==(const CompIterator<IsConst> &, const CompIterator<IsConst> &)
	{
		return false;
	}
	template<bool IsConst>
	std::strong_ordering operator<=>(const CompIterator<IsConst> &l, const CompIterator<IsConst> &r)
	{
		if (auto comp = l.index_ <=> r.index_; comp != 0) return comp;
		return l.target_ <=> r.target_;
	}
	template<bool IsConst>
	auto CompIterator<IsConst>::operator[](int index_) const -> CompIterator<IsConst>::value_type &
	{
		return this->target_->at(index_);
	}

#pragma endregion

}
