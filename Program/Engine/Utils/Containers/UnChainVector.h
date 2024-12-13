/// @file UnChainVector.h
/// @brief 非連続のベクター｡使ってない｡
/// @author ONUKI seiya
#pragma once
#include "../SoLib/SoLib_Traits.h"
#include "ConstVector.h"
#include <array>
#include <memory>
#include <vector>

namespace SoLib {

	template <SoLib::IsNotPointer T>
	class UnChainVector {
	public:
		static constexpr uint32_t kItemCount_ = 16u;

		class iterator {
			friend UnChainVector<T>;
			std::vector<std::unique_ptr<SoLib::ConstVector<T, kItemCount_>>>::iterator outItr_;
			SoLib::ConstVector<T, kItemCount_>::iterator inItr_;

		public:
			iterator() = default;

			bool operator!=(const iterator &other) {
				return outItr_ != other.outItr_ || inItr_ != other.inItr_;
			}

			T &operator*() const {
				return *inItr_;
			}

			iterator &operator++() {
				++inItr_;
				if ((*outItr_)->end() == inItr_) {
					++outItr_;
					inItr_ = (*outItr_)->begin();
				}
			}

		};
	public:

		UnChainVector() = default;
		UnChainVector(const UnChainVector &) = default;
		UnChainVector(UnChainVector &&) = default;

		UnChainVector &operator=(const UnChainVector &) = default;
		UnChainVector &operator=(UnChainVector &&) = default;

		/*template <SoLib::IsContainsType<T> U>
		UntrimmedVector &operator=(const U &u)
		{
			for (size_t i = 0; i < u.size(); i++) {
				(*this)[i] = u[i];
			}
		}*/

		/// @brief 要素の取得
		/// @param index 添え字
		/// @return 対象の要素
		T &operator[](const size_t index)
		{
			return item_[index / kItemCount_]->at(index % kItemCount_);
		}
		const T &operator[](const size_t index) const
		{
			return item_[index / kItemCount_]->at(index % kItemCount_);
		}

		T &push_back(const T &item)
		{
			SoLib::ConstVector<T, kItemCount_> *const backArray = GetBackArray();
			// 要素を追加
			backArray->push_back(item);
		}

		T &push_back(T &&item)
		{
			SoLib::ConstVector<T, kItemCount_> *const backArray = GetBackArray();
			// 要素を追加
			backArray->push_back(std::move(item));
		}

		iterator begin() {
			iterator result{};
			result.outItr_ = item_.begin();
			result.inItr_ = item_.front()->begin();
			return result;
		}

		iterator end() {
			iterator result{};
			result.outItr_ = item_.end();
			return result;
		}


	private:

		SoLib::ConstVector<T, kItemCount_> *const GetBackArray() {

			// もし配列が存在しない場合か、末尾が最大の場合は
			if (not item_.size() || item_.back()->IsMax()) {
				// 要素を追加
				item_.push_back(std::make_unique<SoLib::ConstVector<T, kItemCount_>>());
			}
			SoLib::ConstVector<T, kItemCount_> *const backItem = item_.back().get();

			// 末尾の要素を取得
			return backItem;

		}

	private:
		std::vector<std::unique_ptr<SoLib::ConstVector<T, kItemCount_>>> item_;
	};
}