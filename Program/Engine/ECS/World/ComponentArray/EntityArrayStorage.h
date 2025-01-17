/// @file EntityArrayStorage.h
/// @brief エンティティのデータの管理を行う
/// @author ONUKI seiya
#pragma once
#include <vector>
#include "../../Archetype.h"
#include <memory>
#include <span>
#include "ComponentData.h"

namespace ECS {

	class Chunk;

	class EntityAccessor;

	template<typename T>
	class EntityCompAccessor;

	class EntityClass;

	/// @brief 一つのグループの情報を取得する
	/// @param[in] chunk チャンク
	/// @param[in] index インデックス
	/// @return コンポーネントの参照
	template<typename T>
	T &GetComp(Chunk *chunk, uint32_t index) {
		constexpr uint32_t compId = static_cast<uint32_t>(ECS::ComponentRegistry::GetIndex<T>());
		return *std::bit_cast<T *>(&GetComp(chunk, compId, index));
	}

	/// @brief 一つのグループの情報を取得する
	/// @param[in] chunk チャンク
	/// @param[in] compId コンポーネントID
	/// @param[in] index インデックス
	/// @return コンポーネントへの参照
	std::byte &GetComp(Chunk *chunk, uint32_t compId, uint32_t index);

	/// @brief コンポーネントの配列を取得する
	/// @param[in] chunk チャンク
	/// @param[in] compId コンポーネントID
	/// @return コンポーネントの配列
	ComponentSpan &GetCompArray(Chunk *chunk, uint32_t compId);

	/// @brief コンポーネントの配列を取得する
	/// @param[in] chunk チャンク
	/// @return コンポーネントの配列
	template<typename T>
	ComponentSpan &GetCompArray(Chunk *chunk) {
		constexpr uint32_t compId = static_cast<uint32_t>(ECS::ComponentRegistry::GetIndex<T>());
		return GetCompArray(chunk, compId);
	}

	/// @brief エンティティを移動させる
	/// @param[in] chunk チャンク
	/// @param[in] dst 移動先のインデックス
	/// @param[in] src 移動元のインデックス
	void EntityMove(Chunk *chunk, uint32_t dst, uint32_t src);

	/// @brief エンティティを取得する
	/// @param[in] chunk チャンク
	/// @param[in] index インデックス
	/// @return エンティティ
	EntityClass &GetEntity(Chunk *chunk, uint32_t index);

	class EntityClass {
	public:
		Chunk *chunk_;
		uint32_t totalIndex_; // トータル番号
		uint32_t version_;

		bool operator==(const EntityClass &) const = default;

		explicit operator bool() const { return *this == GetEntity(chunk_, totalIndex_); }

		/// @brief コンポーネントを取得する
		template<typename T>
		T &GetComponent() { return GetComp<T>(chunk_, totalIndex_); }

	};

	class EntityAccessor {
	public:
		Chunk *chunk_;
		uint32_t totalIndex_; // トータル番号
		uint32_t version_;

	private:

	};

	template<typename T>
	class EntityCompAccessor {
	public:
		EntityCompAccessor(const EntityClass &entity, T &comp) : entity_(entity), compData_(comp) {}
		const EntityClass &entity_;
		T &compData_;


		T *operator->() { return &compData_; }
		const T *operator->() const { return &compData_; }

		EntityCompAccessor &operator=(EntityCompAccessor &&move) {
			EntityMove(entity_.chunk_, entity_.totalIndex_, move.entity_.totalIndex_);
			return *this;
		}

	private:

	};


	/// @brief コンポーネントを格納するメモリ
	/// @details 32byteアライメントを行っている
	class alignas(32) EntityComponentMemory : public std::array<std::byte, Archetype::kOneChunkCapacity> {
	public:
		EntityComponentMemory() = default;
		~EntityComponentMemory() = default;
	};

	class EntityArrayStorage
	{
	public:
		EntityArrayStorage() = default;
		EntityArrayStorage(const EntityArrayStorage &) = default;
		EntityArrayStorage &operator=(const EntityArrayStorage &) = default;
		EntityArrayStorage &operator=(EntityArrayStorage &&) = default;
		EntityArrayStorage(Chunk *chunk, uint32_t entCount) : chunk_(chunk), entityCount_(entCount) {}
		~EntityArrayStorage() = default;

	public:

		// エンティティとそれのデータのペア
		using EntityStorage = std::pair<std::unique_ptr<EntityClass[]>, std::unique_ptr<EntityComponentMemory>>;

		// エンティティとコンポーネント情報群へのアクセッサ
		template<bool IsConst = false>
		using EntityData = std::pair<
			std::conditional_t<IsConst, std::span<const EntityClass>, std::span<EntityClass>>,
			std::conditional_t<IsConst, const EntityComponentMemory &, EntityComponentMemory &>
		>;

	public:
		/// @brief グループの追加
		/// @param count 追加数
		std::span<EntityStorage> AddGroup(uint32_t count = 1u);

		/// @brief 値を破棄する
		/// @param flagArray フラグの配列
		/// @param trueCount 生きてる数
		void erase(const std::vector<bool> &flagArray, const size_t trueCount, const uint32_t count);

	public:

		/// @brief エンティティの数
		uint32_t size() const { return static_cast<uint32_t>(entityStorage_.size()); }

		/// @brief エンティティの情報の取得
		/// @return エンティティとコンポーネントの情報の配列
		std::vector<EntityStorage> &GetEntityStorage() { return entityStorage_; }

		/// @brief エンティティの情報の取得
		/// @return エンティティとコンポーネントの情報の配列
		const std::vector<EntityStorage> &GetEntityStorage() const { return entityStorage_; }

		/// @brief 一つのグループの情報を取得する
		/// @param groupId グループ番号
		/// @return グループ情報のペア
		EntityData<false> GetEntityData(uint32_t groupId) {
			const auto &item = entityStorage_.at(groupId);
			EntityData<false> result = {
				std::span<EntityClass>{item.first.get(), entityCount_},
				*item.second
			};
			return result;
		}
		
		/// @brief 一つのグループの情報を取得する
		/// @param groupId グループ番号
		/// @return グループ情報のペア
		const EntityData<true> GetEntityData(uint32_t groupId) const {
			const auto &item = entityStorage_.at(groupId);
			const EntityData<true> result = {
				std::span<const EntityClass>{item.first.get(), entityCount_},
				*item.second
			};
			return result;
		}

		/// @brief エンティティの取得
		/// @param index インデックス
		/// @return エンティティ
		EntityClass &GetEntity(uint32_t index) { return entityStorage_[index / entityCount_].first[index % entityCount_]; }
		/// @brief エンティティの取得
		/// @param index インデックス
		/// @return エンティティ
		const EntityClass &GetEntity(uint32_t index) const { return entityStorage_[index / entityCount_].first[index % entityCount_]; }

		/// @brief 一つのグループに含まれるエンティティの数
		/// @return エンティティの数
		uint32_t GetGroupSize() const { return entityCount_; }

	private:
		Chunk *chunk_;

		// エンティティとメモリの管理
		std::vector<EntityStorage> entityStorage_;
		// 一つのグループに属するエンティティの数
		uint32_t entityCount_;

	};
}