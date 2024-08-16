#pragma once
#include <cstdint>
#include <iterator>
#include <array>
#include "ComponentData.h"
#include "EntityArrayStorage.h"

namespace ECS {

	class Chunk;
	template<typename T>
	class ChunkRange;

	//template<typename T>
	//class ChunkTypeIterator;

	//template<typename T>
	//static inline auto operator++(ChunkTypeIterator<T> &itr)->ChunkTypeIterator<T> &;
	//template<typename T>
	//auto operator++(ChunkTypeIterator<T> &itr, int)->ChunkTypeIterator<T>;
	//template<typename T>
	//auto operator--(ChunkTypeIterator<T> &itr)->ChunkTypeIterator<T> &;
	//template<typename T>
	//auto operator--(ChunkTypeIterator<T> &itr, int)->ChunkTypeIterator<T>;
	//template<typename T>
	//auto operator+(const ChunkTypeIterator<T> &itr, int32_t diff)->ChunkTypeIterator<T>;
	//template<typename T>
	//auto operator+(int32_t diff, const ChunkTypeIterator<T> &itr)->ChunkTypeIterator<T>;
	//template<typename T>
	//auto operator+=(ChunkTypeIterator<T> &itr, int32_t diff)->ChunkTypeIterator<T> &;
	//template<typename T>
	//auto operator-(const ChunkTypeIterator<T> &itr, int32_t diff)->ChunkTypeIterator<T>;
	//template<typename T>
	//auto operator-=(ChunkTypeIterator<T> &itr, int32_t diff)->ChunkTypeIterator<T> &;
	//template<typename T>
	//auto operator-(const ChunkTypeIterator<T> &l, const ChunkTypeIterator<T> &r)->ChunkTypeIterator<T>::difference_type;
	//template<typename T>
	//static inline bool operator==(const ChunkTypeIterator<T> &l, const ChunkTypeIterator<T> &r);
	//template<typename T>
	//std::strong_ordering operator<=>(const ChunkTypeIterator<T> &l, const ChunkTypeIterator<T> &r);

	template<typename T>
	class ChunkTypeIterator {
	public:
		ChunkTypeIterator() = default;
		ChunkTypeIterator(const ChunkTypeIterator &) = default;
		ChunkTypeIterator(ChunkTypeIterator &&) = default;

		ChunkTypeIterator &operator=(const ChunkTypeIterator &) = default;
		ChunkTypeIterator &operator=(ChunkTypeIterator &&) = default;

		ChunkTypeIterator(ChunkRange<T> *chunk, uint32_t index) : chunk_(chunk), index_(index) {}

	public:

		using difference_type = int32_t;
		using value_type = EntityCompAccessor<T>;
		using iterator_category = std::random_access_iterator_tag;

	public:

	/*	friend auto operator++(ChunkTypeIterator<T> &itr)->ChunkTypeIterator<T> &;
		friend auto operator++(ChunkTypeIterator<T> &itr, int)->ChunkTypeIterator<T>;
		friend auto operator--(ChunkTypeIterator<T> &itr)->ChunkTypeIterator<T> &;
		friend auto operator--(ChunkTypeIterator<T> &itr, int)->ChunkTypeIterator<T>;
		friend auto operator+(const ChunkTypeIterator<T> &itr, int32_t diff)->ChunkTypeIterator<T>;
		friend auto operator+(int32_t diff, const ChunkTypeIterator<T> &itr)->ChunkTypeIterator<T>;
		friend auto operator+=(ChunkTypeIterator<T> &itr, int32_t diff)->ChunkTypeIterator<T> &;
		friend auto operator-(const ChunkTypeIterator<T> &itr, int32_t diff)->ChunkTypeIterator<T>;
		friend auto operator-=(ChunkTypeIterator<T> &itr, int32_t diff)->ChunkTypeIterator<T> &;
		friend auto operator-(const ChunkTypeIterator<T> &l, const ChunkTypeIterator<T> &r)->ChunkTypeIterator<T>::difference_type;
		friend bool operator==(const ChunkTypeIterator<T> &l, const ChunkTypeIterator<T> &r);
		friend std::strong_ordering operator<=>(const ChunkTypeIterator<T> &l, const ChunkTypeIterator<T> &r);*/

	public:

		auto operator*()->value_type &;
		auto operator[](uint32_t index) const->value_type &;

	//private:

		// チャンクへのポインタ
		ChunkRange<T> *chunk_;

		// チャンク内でのIndex番号
		uint32_t index_;

	};


	class Chunk;

	template<typename T>
	class ChunkRange {
	public:
		ChunkRange() = default;
		ChunkRange(const ChunkRange &) = default;
		ChunkRange(Chunk *chunk, ComponentData *comp, uint32_t begin, uint32_t end) : chunk_(chunk), compDatas_(comp), begin_(begin), end_(end) {}

		~ChunkRange() = default;

	public:

	public:

		using iterator = ChunkTypeIterator<T>;

	public:

		iterator begin() { return iterator{ this, begin_ }; }

		iterator end() { return iterator{ this, end_ }; }

		ComponentData *GetCompData() { return compDatas_; }
		Chunk *GetChunk() { return chunk_; }

	private:

		Chunk *chunk_;
		ComponentData *compDatas_;

		uint32_t begin_;
		uint32_t end_;

	};


	template<typename T>
	static inline auto operator++(ChunkTypeIterator<T> &itr)->ChunkTypeIterator<T> &{
		itr.index_++;
		return itr;
	}

	template<typename T>
	auto operator++(ChunkTypeIterator<T> &itr, int)->ChunkTypeIterator<T> {
		auto result = itr;
		itr.index_++;
		return result;
	}

	template<typename T>
	auto operator--(ChunkTypeIterator<T> &itr)->ChunkTypeIterator<T> &{
		itr.index_--;
		return itr;
	}

	template<typename T>
	auto operator--(ChunkTypeIterator<T> &itr, int)->ChunkTypeIterator<T> {
		auto result = itr;
		itr.index_--;
		return result;
	}

	template<typename T>
	auto operator+(const ChunkTypeIterator<T> &itr, int32_t diff)->ChunkTypeIterator<T> {
		ChunkTypeIterator<T> result = itr;
		result.index_ += diff;
		return result;
	}

	template<typename T>
	auto operator+(int32_t diff, const ChunkTypeIterator<T> &itr)->ChunkTypeIterator<T> {
		ChunkTypeIterator<T> result = itr;
		result.index_ += diff;
		return result;
	}

	template<typename T>
	auto operator+=(ChunkTypeIterator<T> &itr, int32_t diff)->ChunkTypeIterator<T> &{
		itr.index_ += diff;
		return itr;
	}

	template<typename T>
	auto operator-(const ChunkTypeIterator<T> &itr, int32_t diff)->ChunkTypeIterator<T> {
		ChunkTypeIterator<T> result = itr;
		result.index_ -= diff;
		return result;
	}

	template<typename T>
	auto operator-=(ChunkTypeIterator<T> &itr, int32_t diff)->ChunkTypeIterator<T> &{
		itr.index_ -= diff;
		return itr;
	}

	template<typename T>
	auto operator-(const ChunkTypeIterator<T> &l, const ChunkTypeIterator<T> &r)->ChunkTypeIterator<T>::difference_type {
		return l.index_ - r.index_;
	}

	template<typename T>
	static inline bool operator==(const ChunkTypeIterator<T> &l, const ChunkTypeIterator<T> &r) {
		return l.index_ == r.index_ and l.chunk_ == r.chunk_;
	}

	template<typename T>
	std::strong_ordering operator<=>(const ChunkTypeIterator<T> &l, const ChunkTypeIterator<T> &r) {
		if (auto comp = l.index_ <=> r.index_; comp != 0) return comp;
		return l.chunk_ <=> r.chunk_;
	}

	template<typename T>
	inline auto ChunkTypeIterator<T>::operator*() -> value_type &
	{
		return static_cast<value_type &>(GetEntity(chunk_->GetChunk(), index_));
	}

	template<typename T>
	inline auto ChunkTypeIterator<T>::operator[](uint32_t index) const -> value_type &
	{
		return static_cast<value_type &>(GetEntity(chunk_->GetChunk(), index_));
	}

}
