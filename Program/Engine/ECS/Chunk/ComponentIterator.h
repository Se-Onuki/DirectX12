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
		using ConstRef = std::conditional_t<IsConst, const U &, U &>;
		template<typename U>
		using ConstPtr = std::conditional_t<IsConst, const U *, U *>;

		using difference_type = int32_t;
		using value_type = std::tuple<ConstRef<Ts>...>;
		using iterator_category = std::random_access_iterator_tag;

		using value_ptr = std::tuple<ConstPtr<Ts>...>;

		auto operator*()->value_type;
		auto operator[](uint32_t index) const->value_type;

	private:

		auto GetGroupPtr() const->value_ptr;

	public:

		// エンティティのデータ群
		EntityArrayStorage *pEntityStorage_ = nullptr;
		// メモリの場所
		EntityComponentMemory *pEntityMemory_ = nullptr;

		// それぞれのコンポーネントの始点
		std::array<uint16_t, sizeof...(Ts)> offset_ = { 0u };

		// グループのサイズ
		uint16_t cGroupSize_;

		// Entityの添字
		uint16_t index_;

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
		const auto arraySize = itr.cGroupSize_;

		// もし負数なら､逆側に渡す
		if (diff < 0) { return itr -= -diff; }
		itr.index_ = static_cast<uint16_t>(itr.index_ + diff);
		// もし領域を越えていたら
		if (diff >= arraySize or (itr.index_ % arraySize) < diff) {
			// ストレージ
			auto &groups = itr.pEntityStorage_->GetEntityStorage();

			// ポインタを変更
			itr.pEntityMemory_ = groups.at(itr.index_ / arraySize).second.get();
		}
		return itr;
	}
	template<bool IsConst, typename... Ts>
	auto operator-(const TypeCompIterator<IsConst, Ts...> &itr, int32_t diff) -> TypeCompIterator<IsConst, Ts...>
	{
		TypeCompIterator<IsConst, Ts...> result = itr;
		result -= diff;
		return result;
	}
	template<bool IsConst, typename... Ts>
	auto operator-=(TypeCompIterator<IsConst, Ts...> &itr, int32_t diff) -> TypeCompIterator<IsConst, Ts...> &
	{
		const auto arraySize = itr.cGroupSize_;
		// もし負数なら､逆側に渡す
		if (diff < 0) { return itr += -diff; }
		itr.index_ = static_cast<uint16_t>(itr.index_ - diff);
		// もし領域を越えていたら
		if (diff >= arraySize // もし範囲より大きい移動幅か
			or (itr.index_ % arraySize) > diff	// 移動先が別のグループなら
			) {
			// ストレージ
			auto &groups = itr.pEntityStorage_->GetEntityStorage();

			// ポインタを変更
			itr.pEntityMemory_ = groups.at(itr.index_ / arraySize).second.get();
		}
		return itr;
	}
	template<bool IsConst, typename... Ts>
	auto operator-(const TypeCompIterator<IsConst, Ts...> &l, const TypeCompIterator<IsConst, Ts...> &r) -> TypeCompIterator<IsConst, Ts...>::difference_type
	{
		return l.index_ - r.index_;
	}


	template<bool IsConst, typename... Ts>
	auto TypeCompIterator<IsConst, Ts...>::operator*() -> TypeCompIterator<IsConst, Ts...>::value_type
	{
		// グループ内のIndexに変更する
		uint16_t i = index_ % cGroupSize_;
		return std::apply([i](ConstPtr<Ts>... ptrs) {
			return value_type(ptrs[i]...);
			}, GetGroupPtr());
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
	auto TypeCompIterator<IsConst, Ts...>::operator[](uint32_t index) const -> TypeCompIterator<IsConst, Ts...>::value_type
	{
		return *(*this + index);
	}

	template<bool IsConst, typename ...Ts>
	inline auto TypeCompIterator<IsConst, Ts...>::GetGroupPtr() const -> TypeCompIterator<IsConst, Ts...>::value_ptr
	{
		return std::apply([this](auto... memoryOffsets) {
			return value_ptr(									// 4. それらを全部まとめて返す
				reinterpret_cast<ConstPtr<Ts>>(					// 3. Ts型にポインタを変更する
					&(											// 2. std::byte*に型を変更する
						pEntityMemory_->at(memoryOffsets)	// 1. 保存先の先頭にオフセットを加算する(std::byte&)
						))
				...);
			}, offset_);
	}

#pragma endregion
}