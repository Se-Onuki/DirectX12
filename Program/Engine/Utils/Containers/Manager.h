/// @file Manager.h
/// @brief なにかのマネージャ
/// @author ONUKI seiya
#pragma once
#include "Singleton.h"
#include <memory>
#include <vector>
#include "UnChainVector.h"

namespace SoLib {

	template <SoLib::IsNotPointer Crtp, SoLib::IsNotPointer ItemType>
	class Manager : public SoLib::Singleton<Crtp> {
		friend SoLib::Singleton<Crtp>;
		Manager() = default;
		Manager(const Manager &) = delete;
		Manager &operator=(const Manager &) = delete;
		~Manager() = default;
	public:

		/// @brief 値を取り出すための型
		struct IndexType {
			IndexType(uint32_t index = 0u) : index_(index) {};

			// コピー演算子
			IndexType &operator=(uint32_t index) {
				index_ = index;
				return *this;
			}

			/// @brief indexを取得する
			operator uint32_t() const {
				return index_;
			}
		private:
			// マネージャの添え字
			uint32_t index_;
		};

	public:



		/// @brief アイテムの取得
		/// @param index 添え字
		/// @return アイテムのアドレス
		ItemType *const GetItem(const IndexType index) {
			return manageItems_.at(static_cast<uint32_t>(index)).get();
		}


	private:
		SoLib::UnChainVector<std::unique_ptr<ItemType>> manageItems_;
	};

}