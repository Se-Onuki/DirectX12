/// @file ComponentData.h
/// @brief コンポーネントのSpanのように振る舞う
/// @author ONUKI seiya
#pragma once
#include <span>
#include <cstdint>
#include <vector>
#include <memory>
#include <bit>

#include "ComponentArray.h"

namespace ECS {

	class ComponentSpan;

	class EntityArrayStorage;

	template<typename T, bool IsConst>
	class CompIterator {
	public:

		inline static constexpr bool kIsByte = std::is_same_v<T, std::byte>;

		template<typename U>
		using ConstType = std::conditional_t<IsConst, const U, U>;

		using DataType = std::conditional_t<IsConst, const ComponentSpan *, ComponentSpan *>;

		using difference_type = int32_t;
		using value_type = ConstType<T>;
		using iterator_category = std::random_access_iterator_tag;

		auto operator*()->value_type &;
		auto operator[](uint32_t index) const->value_type &;

		DataType target_;

		uint32_t index_;

	};

	class ComponentSpan {
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
			Range(ComponentSpan *compData, uint32_t begin, uint32_t end) : compData_(compData), begin_(begin), end_(end) {}

			iterator begin() { return iterator{ compData_, begin_ }; }
			const_iterator begin() const { return const_iterator{ compData_, begin_ }; }
			const_iterator cbegin() const { return const_iterator{ compData_, begin_ }; }

			iterator end() { return  iterator{ compData_, end_ }; }
			const_iterator end() const { return const_iterator{ compData_, end_ }; }
			const_iterator cend() const { return const_iterator{ compData_, end_ }; }

			uint32_t size() const { return end_ - begin_; }

		private:

			ComponentSpan *compData_;
			uint32_t begin_;
			uint32_t end_;

		};

		template<typename T, bool IsConst = false>
		class TRange {
		public:

			using iterator = CompIterator<T, false>;
			using const_iterator = CompIterator<T, true>;

			using CompData = std::conditional_t<IsConst, const ComponentSpan *, ComponentSpan *>;

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

			T &front() requires(IsConst == false) { return compData_->at<T>(0); }
			const T &front() const { return compData_->at<T>(0); }

			T &back() requires(IsConst == false) { return compData_->at<T>(end_ - 1); }
			const T &back() const { return compData_->at<T>(end_ - 1); }

			bool IsActive() const { return compData_ and ((end_ - begin_) > 0); }

		private:

			friend TRange<T, true>;
			friend TRange<T, false>;

			CompData compData_;
			uint32_t begin_;
			uint32_t end_;

		};

	public:
		ComponentSpan() = default;
		ComponentSpan(const ComponentSpan &) = default;
		ComponentSpan &operator=(const ComponentSpan &) = default;
		ComponentSpan(EntityArrayStorage *entityStr, uint32_t offset, uint32_t itemSize, uint32_t count) : pEntityStorage_(entityStr), offset_(offset), typeSize_(itemSize), itemCount_(count) {}
		ComponentSpan(uint32_t itemSize, uint32_t count) : typeSize_(itemSize), itemCount_(count) {}

		std::span<std::byte> GetCompArray(uint32_t index);

		std::byte *operator[](uint32_t index);
		const std::byte *operator[](uint32_t index) const;

		std::byte &at(uint32_t index) { return *(*this)[index]; }
		const std::byte &at(uint32_t index) const { return *(*this)[index]; }

		template<typename T>
		T &at(uint32_t index) { return *std::bit_cast<T *>(&at(index)); }
		template<typename T>
		const T &at(uint32_t index) const { return *std::bit_cast<const T *>(&at(index)); }

		/// @brief 範囲を返す
		/// @param end 範囲の終わり
		/// @return データの範囲
		Range View(uint32_t end) { return Range{ this,0,end }; }
		/// @brief 範囲を返す
		/// @param begin 範囲の始まり
		/// @param end 範囲の終わり
		/// @return データの範囲
		Range View(uint32_t begin, uint32_t end) { return Range{ this,begin,end }; }

		/// @brief 範囲を返す
		/// @param end 範囲の終わり
		/// @return 型指定ありのデータの範囲
		template<typename T>
		TRange<T> View(uint32_t end) { return TRange<T>{ this, 0u, end }; }

		/// @brief 範囲を返す
		/// @param begin 範囲の始まり
		/// @param end 範囲の終わり
		/// @return 型指定ありのデータの範囲
		template<typename T>
		TRange<T> View(uint32_t begin, uint32_t end) { return TRange<T>{ this, begin, end }; }


		/// @brief 範囲を返す
		/// @param end 範囲の終わり
		/// @return 型指定ありのデータの範囲
		template<typename T>
		TRange<T, true> View(uint32_t end) const { return TRange<T, true>{ this, 0u, end }; }

		/// @brief 範囲を返す
		/// @param begin 範囲の始まり
		/// @param end 範囲の終わり
		/// @return 型指定ありのデータの範囲
		template<typename T>
		TRange<T, true> View(uint32_t begin, uint32_t end) const { return TRange<T, true>{ this, begin, end }; }

		/// @brief コンポーネントのサイズを返す
		/// @return コンポーネントのサイズ
		uint32_t GetTypeSize() const { return typeSize_; }

		uint32_t GetOffset() const { return offset_; }


	public:

		/// @brief 値を破棄する
		/// @param flagArray フラグの配列
		/// @param trueCount 生きてる数
		void erase(const std::vector<bool> &flagArray, const size_t trueCount, const uint32_t count);


	private:

		// エンティティのデータ群
		EntityArrayStorage *pEntityStorage_ = nullptr;

		// コンポーネントの始点
		uint32_t offset_ = 0u;
		// 形のサイズ
		uint32_t typeSize_{};
		// コンポーネント配列の長さ
		uint32_t itemCount_{};
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
