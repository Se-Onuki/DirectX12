#pragma once
#include "../../Archetype.h"
#include "ComponentArray.h"
#include <unordered_map>

namespace ECS {

	class Chunk {
	public:

		Chunk() = default;
		Chunk(const Archetype &archetype);

		void Init(const Archetype &archetype);

		template<typename T>
		std::vector<std::span<T>> GetComponent();

		const Archetype &GetArchetype() const { return archetype_; }

		template<typename... Ts>
		void push_back(const Ts&... args);

		template<typename... Ts>
		std::tuple<Ts *...> emplace_back();

		uint32_t emplace_back();

	private:

		// 型のデータ
		Archetype archetype_;

		// データを直接管理するもの
		std::vector<std::unique_ptr<std::byte[]>> memData_;

		// メモリの管理 [レジストリ番号, データの位置]
		std::unordered_map<uint32_t, std::vector<std::unique_ptr<ComponentArray>>> compArrays_;

		// 保存しているデータの数
		uint32_t size_;

	private:

		template<typename T>
		decltype(compArrays_)::iterator GetCompArray();

	};


	template<typename T>
	inline std::vector<std::span<T>> Chunk::GetComponent()
	{
		// データの配列を取得する
		decltype(compArrays_)::iterator compTarget = GetCompArray<T>();
		// 存在しなかったら破棄する
		if (compTarget == compArrays_.end()) { return std::vector<std::span<T>>{}; }

		// 1つのデータのエンティティの数
		const uint32_t entCount = archetype_.GetChunkCapacity();

		// 返すデータ
		std::vector<std::span<T>> result{ compTarget->second.size() };

		// コンポーネントの配列にして返す
		std::transform(compTarget->second.begin(), compTarget->second.end(), result.begin(), [](const std::unique_ptr<ComponentArray> &item) { return item->GetArray<T>(); });

		// 末尾のデータを短くなるよう書き換える
		result.back() = { reinterpret_cast<T *>(compTarget->second.back()->data()), size_ % entCount };

		// 情報を渡す
		return result;
	}

	template<typename ...Ts>
	inline void Chunk::push_back(const Ts & ...args)
	{
		// 1つのデータのエンティティの数
		const uint32_t entCount = archetype_.GetChunkCapacity();

		// コンポーネントの配列を取得し､その配列にデータを保存する
		// { GetCompArray<Ts>()->second[size_ / entCount]->GetArray<Ts>()[size_ % entCount] = std::forward<const Ts &>(args); }...;

		(
			(
				[this, entCount, args]()
					{
						GetCompArray<Ts>()->second[size_ / entCount]->GetArray<Ts>()[size_ % entCount] = std::forward<const Ts &>(args);
					}()
			), ...
		);
		Entity *entity = reinterpret_cast<ECS::Entity *>(memData_[size_ / entCount].get()) + size_ % entCount;
		entity->totalIndex_ = size_;
		entity->version_++;

		size_++;
	}

	template<typename ...Ts>
	inline std::tuple<Ts *...> Chunk::emplace_back()
	{
		// 1つのデータのエンティティの数
		const uint32_t entCount = archetype_.GetChunkCapacity();

		const auto *const compRegistry = ECS::ComponentRegistry::GetInstance();

		// コンポーネントの配列を取得し､その配列にデータを保存する
		for (auto &[key, comps] : compArrays_) {
			// コンストラクタを呼び出して実行する
			compRegistry->typeDatas_[key].constructor_(comps[size_ / entCount]->at(size_ % entCount));
		}

		using Result = std::tuple<Ts*...>;


		// コンポーネントの配列を取得し､その配列にデータを保存する
		Result result = std::make_tuple(
			(

				[this, entCount]()
				{
					return (&(GetCompArray<Ts>()->second[size_ / entCount]->GetArray<Ts>()[size_ % entCount]));
				}()
					)...);

		Entity *entity = reinterpret_cast<ECS::Entity *>(memData_[size_ / entCount].get()) + size_ % entCount;
		entity->totalIndex_ = size_;
		entity->version_++;

		size_++;

		return result;
	}

	inline uint32_t Chunk::emplace_back()
	{
		// 1つのデータのエンティティの数
		const uint32_t entCount = archetype_.GetChunkCapacity();

		const auto *const compRegistry = ECS::ComponentRegistry::GetInstance();

		// コンポーネントの配列を取得し､その配列にデータを保存する
		for (auto &[key, comps] : compArrays_) {
			// コンストラクタを呼び出して実行する
			compRegistry->typeDatas_[key].constructor_(comps[size_ / entCount]->at(size_ % entCount));
		}
		Entity *entity = reinterpret_cast<ECS::Entity *>(memData_[size_ / entCount].get()) + size_ % entCount;
		entity->totalIndex_ = size_;
		entity->version_++;

		return size_++;
	}

	template<typename T>
	inline decltype(Chunk::compArrays_)::iterator Chunk::GetCompArray()
	{
		return compArrays_.find(ECS::ComponentRegistry::GetIndex<T>());
	}

}