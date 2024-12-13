/// @file ChunkIterator.h
/// @brief チャンクのイテレータ
/// @author ONUKI seiya
#pragma once
#include <cstdint>
#include <compare>
#include <iterator>
#include "EntityArrayStorage.h"

namespace ECS {

	class Chunk;

	class ChunkEntityAccessor
	{
	public:


	public:
		ChunkEntityAccessor() = default;
		ChunkEntityAccessor(Chunk *chunk, uint32_t index) : chunk_(chunk), index_(index) {};
		~ChunkEntityAccessor() = default;

		ChunkEntityAccessor &operator=(const ChunkEntityAccessor &) = delete;
		ChunkEntityAccessor &operator=(ChunkEntityAccessor &&move) noexcept;

	private:
		Chunk *chunk_ = nullptr;
		uint32_t index_ = 0u;
	};

	class ChunkIterator {
	public:
		ChunkIterator() = default;
		ChunkIterator(const ChunkIterator &) = default;
		ChunkIterator &operator=(const ChunkIterator &) = default;
		ChunkIterator(ChunkIterator &&) = default;
		ChunkIterator &operator=(ChunkIterator &&) = default;
		ChunkIterator(Chunk *chunk, uint32_t index) : chunk_(chunk), index_(index) {}

	public:

		using difference_type = int32_t;
		using value_type = EntityClass;
		using iterator_category = std::random_access_iterator_tag;

	public:

		friend auto operator++(ChunkIterator &itr)->ChunkIterator &;
		friend auto operator++(ChunkIterator &itr, int)->ChunkIterator;
		friend auto operator--(ChunkIterator &itr)->ChunkIterator &;
		friend auto operator--(ChunkIterator &itr, int)->ChunkIterator;
		friend auto operator+(const ChunkIterator &itr, int32_t diff)->ChunkIterator;
		friend auto operator+(int32_t diff, const ChunkIterator &itr)->ChunkIterator;
		friend auto operator+=(ChunkIterator &itr, int32_t diff)->ChunkIterator &;
		friend auto operator-(const ChunkIterator &itr, int32_t diff)->ChunkIterator;
		friend auto operator-=(ChunkIterator &itr, int32_t diff)->ChunkIterator &;
		friend auto operator-(const ChunkIterator &l, const ChunkIterator &r)->ChunkIterator::difference_type;
		friend bool operator==(const ChunkIterator &l, const ChunkIterator &r);
		friend std::strong_ordering operator<=>(const ChunkIterator &l, const ChunkIterator &r);

	public:

		auto operator*()->value_type&;
		auto operator[](uint32_t index) const->value_type&;

	private:

		// チャンクへのポインタ
		Chunk *chunk_;

		// チャンク内でのIndex番号
		uint32_t index_;

	};

	auto operator++(ChunkIterator &itr)->ChunkIterator &;

	auto operator++(ChunkIterator &itr, int)->ChunkIterator;

	auto operator--(ChunkIterator &itr)->ChunkIterator &;

	auto operator--(ChunkIterator &itr, int)->ChunkIterator;

	auto operator+(const ChunkIterator &itr, int32_t diff)->ChunkIterator;

	auto operator+(int32_t diff, const ChunkIterator &itr)->ChunkIterator;

	auto operator+=(ChunkIterator &itr, int32_t diff)->ChunkIterator &;

	auto operator-(const ChunkIterator &itr, int32_t diff)->ChunkIterator;

	auto operator-=(ChunkIterator &itr, int32_t diff)->ChunkIterator &;

	auto operator-(const ChunkIterator &l, const ChunkIterator &r)->ChunkIterator::difference_type;

	bool operator==(const ChunkIterator &l, const ChunkIterator &r);

	std::strong_ordering operator<=>(const ChunkIterator &l, const ChunkIterator &r);

}