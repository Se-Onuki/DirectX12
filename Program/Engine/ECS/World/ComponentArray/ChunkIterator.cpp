#include "ChunkIterator.h"
#include "Chunk.h"

namespace ECS {

	auto operator++(ChunkIterator &itr) -> ChunkIterator &
	{
		itr.index_++;
		return itr;
	}

	auto operator++(ChunkIterator &itr, int) -> ChunkIterator
	{
		auto result = itr;
		itr.index_++;
		return result;
	}

	auto operator--(ChunkIterator &itr) -> ChunkIterator &
	{
		itr.index_--;
		return itr;
	}

	auto operator--(ChunkIterator &itr, int) -> ChunkIterator
	{
		auto result = itr;
		itr.index_--;
		return result;
	}

	auto operator+(const ChunkIterator &itr, int32_t diff) -> ChunkIterator
	{
		ChunkIterator result = itr;
		result.index_ += diff;
		return result;
	}

	auto operator+(int32_t diff, const ChunkIterator &itr) -> ChunkIterator
	{
		ChunkIterator result = itr;
		result.index_ += diff;
		return result;
	}

	auto operator+=(ChunkIterator &itr, int32_t diff) -> ChunkIterator &
	{
		itr.index_ += diff;
		return itr;
	}

	auto operator-(const ChunkIterator &itr, int32_t diff) -> ChunkIterator
	{
		ChunkIterator result = itr;
		result.index_ -= diff;
		return result;
	}

	auto operator-=(ChunkIterator &itr, int32_t diff) -> ChunkIterator &
	{
		itr.index_ -= diff;
		return itr;
	}

	auto operator-(const ChunkIterator &l, const ChunkIterator &r) -> ChunkIterator::difference_type
	{
		return l.index_ - r.index_;
	}

	bool operator==(const ChunkIterator &l, const ChunkIterator &r)
	{
		return l.index_ == r.index_ and l.chunk_ == r.chunk_;
	}

	std::strong_ordering operator<=>(const ChunkIterator &l, const ChunkIterator &r)
	{
		if (auto comp = l.index_ <=> r.index_; comp != 0) return comp;
		return l.chunk_ <=> r.chunk_;
	}

	auto ChunkIterator::operator*() -> ChunkIterator::value_type &
	{
		return chunk_->GetEntity(index_);
	}

	auto ChunkIterator::operator[](uint32_t index) const -> ChunkIterator::value_type &
	{
		return chunk_->GetEntity(index);
	}

	/// @brief データのムーブコピーを行う
	/// @param move 参照元
	/// @return 
	ChunkEntityAccessor &ChunkEntityAccessor::operator=(ChunkEntityAccessor &&move) noexcept
	{
		// コンポーネントの配列にアクセスして､データを書き換えてコピーする
		for (auto &[key, comp] : chunk_->componentDatas_) {
			// 自分自身のメモリ
			std::byte *dst = &comp.at(index_);
			// 相手のメモリ
			std::byte *src = &comp.at(move.index_);

			// メモリを書き換えて渡す
			std::memcpy(dst, src, comp.GetTypeSize());
		}

		// バージョンを加算する
		chunk_->GetEntity(index_).version_++;

		return *this;
	}

}