#pragma once
#include "../../Archetype.h"
#include "ComponentArray.h"
#include <unordered_map>
#include "EntityArrayStorage.h"
#include "ComponentData.h"

namespace ECS {

	class Chunk {
	public:

		Chunk() = default;
		Chunk(const Archetype &archetype);

		void Init(const Archetype &archetype);

		template<typename T>
		ComponentData::TRange<T> GetComponent();

		const Archetype &GetArchetype() const { return archetype_; }

		uint32_t size() const { return size_; }

	public:

		template<typename... Ts>
		void push_back(const Ts&... args);

		template<typename... Ts>
		std::tuple<Ts *...> emplace_back();

		uint32_t emplace_back();

	public:

		std::byte *GetComp(uint32_t compId, uint32_t index);
		template<typename T>
		T *GetComp(uint32_t index);

	private:

		// 型のデータ
		Archetype archetype_;

		// 情報の保存先
		EntityArrayStorage storage_;

		// コンポーネントへのアクセッサ
		std::unordered_map<uint32_t, ComponentData> componentDatas_;

		// 保存しているデータの数
		uint32_t size_;

	private:

		void AddGroups(const uint32_t count = 1u);

		template<typename T>
		decltype(componentDatas_)::iterator GetCompArray();

	};


	template<typename T>
	inline ComponentData::TRange<T> Chunk::GetComponent()
	{
		// データの配列を取得する
		auto compTarget = GetCompArray<T>();
		// 存在しなかったら破棄する
		if (compTarget == componentDatas_.end()) { return {}; }

		// 返すデータ
		ComponentData::TRange<T> result = compTarget->second.View<T>(size_);

		// 情報を渡す
		return result;
	}

	template<typename ...Ts>
	inline void Chunk::push_back(const Ts & ...args)
	{

		// エンティティのID
		const uint32_t entityId = emplace_back();

		(
			(
				[this, entityId, args]()
				{
					GetCompArray<Ts>()->second.at<Ts>(entityId) = std::forward<const Ts &>(args);
				}()
					), ...
			);

	}

	template<typename ...Ts>
	inline std::tuple<Ts *...> Chunk::emplace_back()
	{
		// エンティティのID
		const uint32_t entityId = emplace_back();

		using Result = std::tuple<Ts*...>;

		// コンポーネントの配列を取得し､その配列にデータを保存する
		Result result = std::make_tuple(
			(

				[this, entityId]()
				{
					return (&(GetCompArray<Ts>()->second.at<Ts>(entityId)));
				}()
					)...
		);

		return result;
	}

	inline uint32_t Chunk::emplace_back()
	{
		// 1つのデータのエンティティの数
		const uint32_t entCount = archetype_.GetChunkCapacity();

		// もし末尾まで到達していたら
		if (entCount * storage_.size() <= size_) {
			// メモリを確保する
			AddGroups();
		}

		const auto *const compRegistry = ECS::ComponentRegistry::GetInstance();

		// コンポーネントの配列を取得し､その配列にデータを保存する
		for (auto &[key, comps] : componentDatas_) {
			// コンストラクタを呼び出して実行する
			compRegistry->typeDatas_[key].constructor_(comps[size_]);
		}

		// エンティティの取得
		Entity &entity = storage_.GetEntity(size_);
		entity.totalIndex_ = size_;
		entity.version_++;

		return size_++;
	}

	inline std::byte *Chunk::GetComp(uint32_t compId, uint32_t index)
	{
		return componentDatas_[compId][index];
	}


	template<typename T>
	inline T *Chunk::GetComp(uint32_t index)
	{
		// 1つのデータのエンティティの数
		const uint32_t entCount = archetype_.GetChunkCapacity();
		return &GetCompArray<T>()->second.at<T>(index);
	}


	template<typename T>
	inline decltype(Chunk::componentDatas_)::iterator Chunk::GetCompArray()
	{
		return componentDatas_.find(static_cast<uint32_t>(ECS::ComponentRegistry::GetIndex<T>()));
	}

}