/// @file ComponentIterator.h
/// @brief コンポーネントのイテレータ
/// @author ONUKI seiya
#pragma once
#include <concepts>
#include <cstdint>
#include <array>

#include "../World/ComponentArray/EntityArrayStorage.h"

namespace ECS {
	template<bool IsConst, typename... Ts>
	class TypeCompIterator {
	public:

		template<typename U>
		using ConstType = std::conditional_t<IsConst, const U &, U &>;

		using difference_type = int32_t;
		using value_type = std::tuple<ConstType<Ts>...>;
		using iterator_category = std::random_access_iterator_tag;

		auto operator*()->value_type;
		auto operator[](uint32_t index) const->value_type;

	private:

		template<std::size_t... Indices>
		auto CreateValue(std::index_sequence<Indices...>, uint16_t i) const->value_type;

	public:

		// エンティティのデータ群
		EntityArrayStorage *pEntityStorage_ = nullptr;
		// メモリの場所
		EntityComponentMemory *pEntityMemory_ = nullptr;

		// それぞれのコンポーネントの始点
		std::array<uint16_t, sizeof...(Ts)> offset_ = { 0u };

		// Entityの添字
		uint16_t index_;

		// グループのサイズ
		uint16_t cGroupSize_;

		// 全体のサイズ
		uint16_t cSize_;

	};

#pragma region Func

	template<bool IsConst, typename... Ts>
	auto operator++(TypeCompIterator<IsConst, Ts...> &itr) -> TypeCompIterator<IsConst, Ts...> &
	{
		// 次に進む
		itr.index_++;
		// もし末尾に到達していたら
		if (itr.index_ % itr.cGroupSize_ == 0u) {
			// ストレージ
			auto &groups = itr.pEntityStorage_->GetEntityStorage();

			// ポインタを変更
			itr.pEntityMemory_ = groups.at(itr.index_ / itr.cGroupSize_).second.get();
		}
		return itr;
	}

	template<bool IsConst, typename... Ts>
	auto operator++(TypeCompIterator<IsConst, Ts...> &itr, int) -> TypeCompIterator<IsConst, Ts...>
	{
		auto result = itr;
		++itr;
		return result;
	}

	template<bool IsConst, typename... Ts>
	auto operator--(TypeCompIterator<IsConst, Ts...> &itr) -> TypeCompIterator<IsConst, Ts...> &
	{
		// 次に進む
		itr.index_--;
		// もし末尾に到達していたら
		if ((itr.index_ + 1) % itr.cGroupSize_ == 0) {
			// ストレージ
			auto &groups = itr.pEntityStorage_->GetEntityStorage();

			// ポインタを変更
			itr.pEntityMemory_ = groups.at(itr.index_ / itr.cGroupSize_).second.get();
		}
		return itr;
	}

	template<bool IsConst, typename... Ts>
	auto operator--(TypeCompIterator<IsConst, Ts...> &itr, int) -> TypeCompIterator<IsConst, Ts...>
	{
		auto result = itr;
		--itr;
		return result;
	}

	template<bool IsConst, typename... Ts>
	auto operator+(const TypeCompIterator<IsConst, Ts...> &itr, int32_t diff) -> TypeCompIterator<IsConst, Ts...>
	{
		auto result = itr;
		result += diff;

		return result;
	}
	template<bool IsConst, typename... Ts>
	auto operator+(int32_t diff, const TypeCompIterator<IsConst, Ts...> &itr) -> TypeCompIterator<IsConst, Ts...>
	{
		auto result = itr;
		result += diff;
		return result;
	}
	template<bool IsConst, typename... Ts>
	auto operator+=(TypeCompIterator<IsConst, Ts...> &itr, int32_t diff) -> TypeCompIterator<IsConst, Ts...> &
	{
		// もし負数なら､逆側に渡す
		if (diff < 0) { return itr -= -diff; }
		itr.index_ += diff;
		// もし領域を越えていたら
		if (diff >= itr.cGroupSize_ or (itr.index_ % itr.cGroupSize_) < diff) {
			// ストレージ
			auto &groups = itr.pEntityStorage_->GetEntityStorage();

			// ポインタを変更
			itr.pEntityMemory_ = groups.at(itr.index_ / itr.cGroupSize_).second.get();
		}
		return itr;
	}
	template<bool IsConst, typename... Ts>
	auto operator-(const TypeCompIterator<IsConst, Ts...> &itr, int32_t diff) -> TypeCompIterator<IsConst, Ts...>
	{
		TypeCompIterator<IsConst, Ts...> result = itr;
		result.index_ -= diff;
		return result;
	}
	template<bool IsConst, typename... Ts>
	auto operator-=(TypeCompIterator<IsConst, Ts...> &itr, int32_t diff) -> TypeCompIterator<IsConst, Ts...> &
	{
		// もし負数なら､逆側に渡す
		if (diff < 0) { return itr += -diff; }
		itr.index_ -= diff;
		// もし領域を越えていたら
		if (diff >= itr.cGroupSize_ // もし範囲より大きい移動幅か
			or (itr.index_ % itr.cGroupSize_) > diff	// 移動先が別のグループなら
			) {
			// ストレージ
			auto &groups = itr.pEntityStorage_->GetEntityStorage();

			// ポインタを変更
			itr.pEntityMemory_ = groups.at(itr.index_ / itr.cGroupSize_).second.get();
		}
		return itr;
	}
	template<bool IsConst, typename... Ts>
	auto operator-(const TypeCompIterator<IsConst, Ts...> &l, const TypeCompIterator<IsConst, Ts...> &r) -> TypeCompIterator<IsConst, Ts...>::difference_type
	{
		return l.index_ - r.index_;
	}

	template<bool IsConst, typename... Ts>
	template<std::size_t... Indices>
	auto TypeCompIterator<IsConst, Ts...>::CreateValue(std::index_sequence<Indices...>, uint16_t i) const -> value_type
	{
		return { (*(std::bit_cast<Ts *>(pEntityMemory_->at(offset_[Indices])) + i))... };
	}

	template<bool IsConst, typename... Ts>
	auto TypeCompIterator<IsConst, Ts...>::operator*() -> TypeCompIterator<IsConst, Ts...>::value_type
	{
		uint16_t i = index_ % cGroupSize_;
		return std::apply([this, i](auto... memoryOffsets) {
			return value_type(*(reinterpret_cast<Ts *>(&(pEntityMemory_->at(memoryOffsets))) + i)...);
			}, offset_);
		//return CreateValue(std::index_sequence_for<Ts...>(), i);
	}

	template<bool IsConst, typename... Ts>
	bool operator==(const TypeCompIterator<IsConst, Ts...> &l, const TypeCompIterator<IsConst, Ts...> &r)
	{
		return l.index_ == r.index_;
	}
	template<bool IsConst, typename... Ts>
	std::strong_ordering operator<=>(const TypeCompIterator<IsConst, Ts...> &l, const TypeCompIterator<IsConst, Ts...> &r)
	{
		return l.index_ <=> r.index_;
	}

	template<bool IsConst, typename... Ts>
	auto TypeCompIterator<IsConst, Ts...>::operator[](uint32_t index_) const -> TypeCompIterator<IsConst, Ts...>::value_type
	{
		return *(*this + index_);
	}

#pragma endregion
}