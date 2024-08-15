#pragma once
#include <vector>
#include "../../Entity/Entity.hpp"
#include "../../Archetype.h"
#include <memory>
#include <span>

namespace ECS {
	class EntityArrayStorage
	{
	public:
		EntityArrayStorage() = default;
		EntityArrayStorage(const EntityArrayStorage &) = default;
		EntityArrayStorage &operator=(const EntityArrayStorage &) = default;
		EntityArrayStorage &operator=(EntityArrayStorage &&) = default;
		EntityArrayStorage(uint32_t entCount) :entityCount_(entCount) {}
		~EntityArrayStorage() = default;

	public:

		// エンティティとそれのデータのペア
		using EntityStorage = std::pair<std::unique_ptr<Entity[]>, std::unique_ptr<std::array<std::byte, Archetype::kOneChunkCapacity>>>;

		// エンティティとコンポーネント情報群へのアクセッサ
		template<bool IsConst = false>
		using EntityData = std::pair<
			std::conditional_t<IsConst, std::span<const Entity>, std::span<Entity>>,
			std::conditional_t<IsConst, const std::array<std::byte, Archetype::kOneChunkCapacity> &, std::array<std::byte, Archetype::kOneChunkCapacity> &>
		>;

	public:
		/// @brief グループの追加
		/// @param count 追加数
		std::span<EntityStorage> AddGroup(uint32_t count = 1u);

	public:

		uint32_t size() const { return static_cast<uint32_t>(entityStorage_.size()); }

		/// @brief エンティティの情報の取得
		/// @return エンティティとコンポーネントの情報の配列
		const std::vector<EntityStorage> &GetEntityStorage() const { return entityStorage_; }

		/// @brief 一つのグループの情報を取得する
		/// @param groupId グループ番号
		/// @return グループ情報のペア
		EntityData<false> GetEntityData(uint32_t groupId) {
			const auto &item = entityStorage_.at(groupId);
			EntityData<false> result = {
				std::span<Entity>{item.first.get(), entityCount_},
				*item.second
			};
			return result;
		}
		const EntityData<true> GetEntityData(uint32_t groupId) const {
			const auto &item = entityStorage_.at(groupId);
			const EntityData<true> result = {
				std::span<const Entity>{item.first.get(), entityCount_},
				*item.second
			};
			return result;
		}

		Entity &GetEntity(uint32_t index) { return entityStorage_[index / entityCount_].first[index % entityCount_]; }
		const Entity &GetEntity(uint32_t index) const { return entityStorage_[index / entityCount_].first[index % entityCount_]; }

	private:

		// エンティティとメモリの管理
		std::vector<EntityStorage> entityStorage_;
		// 一つのグループに属するエンティティの数
		uint32_t entityCount_;

	};
}