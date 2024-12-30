/// @file Chunk.h
/// @brief チャンク
/// @author ONUKI seiya
#pragma once
#include "../../Archetype.h"
#include "ChunkIterator.h"
#include "ChunkRange.h"
#include "ComponentArray.h"
#include "ComponentData.h"
#include "EntityArrayStorage.h"
#include <unordered_map>

namespace ECS {

	class Chunk
	{
	public:
		Chunk() = default;
		Chunk(const Archetype &archetype);

		/// @brief 初期化
		/// @param[in] archetype 初期化するアーキテクチャ
		void Init(const Archetype &archetype);

	public:
		friend ChunkEntityAccessor;
		friend void EntityMove(Chunk *, uint32_t, uint32_t);
		friend std::byte &ECS::GetComp(Chunk *, uint32_t, uint32_t);
		friend ECS::ComponentSpan &ECS::GetCompArray(ECS::Chunk *chunk, uint32_t compId);

	public:
		using iterator = ChunkIterator;

	public:
		/// @brief T型のコンポーネントのRangeを取得
		/// @tparam T 取得するコンポーネント
		/// @return コンポーネントのRange
		template <typename T>
		ChunkRange<T> View() { return {this, &(GetCompArray<T>()->second), 0u, size_}; }

		/// @brief 複数のコンポーネントのRangeを取得
		/// @tparam Ts 取得するコンポーネント
		/// @return コンポーネントのRange
		template <typename... Ts>
			requires(sizeof...(Ts) >= 2)
		ChunkRange<Ts...> View()
		{
			return {this, std::tuple<ComponentSpan *...>{&(GetCompArray<Ts>()->second)...}, 0u, size_};
		}

		/// @brief 開始イテレータの取得
		iterator begin() { return iterator{this, 0}; }

		/// @brief 番兵イテレータの取得
		iterator end() { return iterator{this, size_}; }

		/// @brief コンポーネントの取得
		/// @param[in] compId コンポーネントのID
		/// @return コンポーネント
		ComponentSpan *GetComponent(uint32_t compId);
		/// @brief コンポーネントの取得
		/// @param[in] compId コンポーネントのID
		/// @return コンポーネントのRange
		ComponentSpan::Range GetComponentRange(uint32_t compId);
		/// @brief コンポーネントの取得
		/// @tparam T コンポーネントの型
		/// @return コンポーネントのTRange
		template <typename T>
		ComponentSpan::TRange<T> GetComponent();
		/// @brief コンポーネントの取得
		/// @tparam T コンポーネントの型
		/// @return コンポーネントのTRange
		template <typename T>
		ComponentSpan::TRange<T, true> GetComponent() const;

		/// @brief アーキタイプの取得
		/// @return アーキタイプ
		const Archetype &GetArchetype() const { return archetype_; }

		/// @brief サイズの取得
		/// @return サイズ
		/// @details エンティティの数
		uint32_t size() const { return size_; }

	public:
		/// @brief エンティティの追加
		/// @tparam Ts 追加するコンポーネント
		/// @param[in] args 追加するコンポーネント
		template <typename... Ts>
		void push_back(const Ts &...args);

		/// @brief エンティティの構築
		/// @tparam ...Ts コンポーネントの型
		/// @return このエンティティのコンポーネント
		template <typename... Ts>
		std::tuple<Ts *...> emplace_back();

		/// @brief エンティティの構築
		/// @return このエンティティのIndex
		uint32_t emplace_back();
		/// @brief エンティティの構築
		/// @param[in] count エンティティの数
		/// @return このエンティティのIndexの幅
		std::pair<uint32_t, uint32_t> emplace_back(const uint32_t count);

		/// @brief エンティティの削除
		/// @tparam T 検知するコンポーネント
		/// @param[in] pred 削除するかどうかを判断する関数
		template <typename T, typename Predicate>
		void erase_if(const Predicate &pred);

		/// @brief 一致した値の並びと数を返す
		/// @tparam T 比較する型
		/// @param data 比較する値
		/// @return 一致しているかをtrueで返す
		template <typename T, typename Predicate>
		std::pair<std::vector<bool>, size_t> CountIfFlag(const Predicate &pred) const;

	public:
		/// @brief コンポーネントの取得
		/// @param compId コンポーネントのID
		/// @param index エンティティの番号
		/// @return データのアドレス
		std::byte *GetComp(uint32_t compId, uint32_t index);
		/// @brief コンポーネントの取得
		/// @tparam T コンポーネントの型
		/// @param index エンティティの番号
		/// @return コンポーネントのアドレス
		template <typename T>
		T *GetComp(uint32_t index);

		/// @brief エンティティの取得
		/// @param[in] index エンティティの番号
		/// @return エンティティ
		EntityClass &GetEntity(uint32_t index) { return storage_->GetEntity(index); }
		const EntityClass &GetEntity(uint32_t index) const { return storage_->GetEntity(index); }

	public:
		/// @brief サイズの変更
		/// @param size 変更後のサイズ
		void Resize(uint32_t size) { size_ = size; }

	private:
		// 型のデータ
		Archetype archetype_;

		// 情報の保存先
		std::unique_ptr<EntityArrayStorage> storage_;

		// コンポーネントへのアクセッサ
		std::unordered_map<uint32_t, ComponentSpan> componentDatas_;

		// 保存しているデータの数
		uint32_t size_ = 0u;

	private:
		/// @brief グループの追加
		/// @param count 追加する数
		void AddGroups(const uint32_t count = 1u);

		/// @brief コンポーネントの配列を取得する
		/// @tparam T コンポーネントの型
		/// @return Tに対応するコンポーネントの配列
		template <typename T>
		decltype(componentDatas_)::iterator GetCompArray();

		/// @brief コンポーネントの配列を取得する
		/// @tparam T コンポーネントの型
		/// @return Tに対応するコンポーネントの配列
		template <typename T>
		decltype(componentDatas_)::const_iterator GetCompArray() const;
	};

	inline ComponentSpan *Chunk::GetComponent(uint32_t compId)
	{
		return &componentDatas_[compId];
	}

	inline ComponentSpan::Range Chunk::GetComponentRange(uint32_t compId)
	{
		return componentDatas_[compId].View(size_);
	}

	template <typename T>
	inline ComponentSpan::TRange<T> Chunk::GetComponent()
	{
		// データの配列を取得する
		auto compTarget = GetCompArray<T>();
		// 存在しなかったら破棄する
		if (compTarget == componentDatas_.end()) {
			return {};
		}

		// 返すデータ
		ComponentSpan::TRange<T> result = compTarget->second.View<T>(size_);

		// 情報を渡す
		return result;
	}

	template <typename T>
	inline ComponentSpan::TRange<T, true> Chunk::GetComponent() const
	{
		// データの配列を取得する
		auto compTarget = GetCompArray<T>();
		// 存在しなかったら破棄する
		if (compTarget == componentDatas_.end()) {
			return {};
		}

		// 返すデータ
		ComponentSpan::TRange<T, true> result = compTarget->second.View<T>(size_);

		// 情報を渡す
		return result;
	}

	template <typename... Ts>
	inline void Chunk::push_back(const Ts &...args)
	{

		// エンティティのID
		const uint32_t entityId = emplace_back();

		(
			(
				[this, entityId, args]() {
					GetCompArray<Ts>()->second.at<Ts>(entityId) = std::forward<const Ts &>(args);
				}()),
			...);
	}

	template <typename... Ts>
	inline std::tuple<Ts *...> Chunk::emplace_back()
	{
		// エンティティのID
		const uint32_t entityId = emplace_back();

		using Result = std::tuple<Ts *...>;

		// コンポーネントの配列を取得し､その配列にデータを保存する
		Result result = std::make_tuple(
			(

				[this, entityId]() {
					return (&(GetCompArray<Ts>()->second.at<Ts>(entityId)));
				}())...);

		return result;
	}

	template <typename T, typename Predicate>
	inline void Chunk::erase_if(const Predicate &pred)
	{
		// もし何も持ってなかったら終わる｡
		if (not this->size()) {
			return;
		}
		const auto &[flag, // 関数に対する正負値
					 count // 一致した数
		] = this->CountIfFlag<T>(pred);

		// もし何も一致しなかったら終わり
		if (not count) {
			return;
		}
		if (count != size()) {

			// それぞれのコンポーネントで操作を行う
			for (auto &[key, compArray] : this->componentDatas_) {
				// エンティティの生存数に応じて並べ替える
				compArray.erase(flag, count, size_);
			}
			// エンティティの並べ替えをする
			storage_->erase(flag, count, size_);
		}
		// 死んだ数で除算
		size_ -= static_cast<uint32_t>(count);
	}

	template <typename T, typename Predicate>
	inline std::pair<std::vector<bool>, size_t> Chunk::CountIfFlag(const Predicate &pred) const
	{
		// ヒット数
		size_t count = 0;
		// 要素数の数だけのメモリを確保する
		std::vector<bool> flag(size());
		auto itr = flag.begin();
		// チャンク内部を走査
		// コンポーネントを取得して返す
		for (const T &value : this->GetComponent<T>()) {
			// 条件に一致したらtrueを代入し､カウントを追加する
			if (*itr++ = pred(value)) {
				count++;
			}
		}
		return {std::move(flag), count};
	}

	inline uint32_t Chunk::emplace_back()
	{
		// 1つのデータのエンティティの数
		const uint32_t entCount = archetype_.GetChunkCapacity();

		// もし末尾まで到達していたら
		if (entCount * storage_->size() <= size_) {
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
		EntityClass &entity = storage_->GetEntity(size_);
		entity.totalIndex_ = size_;
		entity.version_++;

		return size_++;
	}

	inline std::pair<uint32_t, uint32_t> Chunk::emplace_back(const uint32_t count)
	{
		// 1つのデータのエンティティの数
		const uint32_t entCount = archetype_.GetChunkCapacity();

		// もし末尾まで到達していたら
		if (entCount * storage_->size() < size_ + count) {
			// メモリを確保する
			AddGroups();
		}

		const uint32_t beforeCount = size_;

		const auto *const compRegistry = ECS::ComponentRegistry::GetInstance();

		// コンポーネントの配列を取得し､その配列にデータを保存する
		for (auto &[key, comps] : componentDatas_) {

			auto constructor = compRegistry->typeDatas_[key].constructor_;

			for (uint32_t i = 0; i < count; i++) {
				auto target = comps[size_ + i];
				constructor(target);
			}
		}

		for (uint32_t i = 0; i < count; i++) {
			// エンティティの取得
			EntityClass &entity = storage_->GetEntity(size_ + i);
			entity.version_++;
		}
		size_ += count;
		const std::pair<uint32_t, uint32_t> result{beforeCount, size_};

		return result;
	}

	inline std::byte *Chunk::GetComp(uint32_t compId, uint32_t index)
	{
		return componentDatas_[compId][index];
	}

	template <typename T>
	inline T *Chunk::GetComp(uint32_t index)
	{
		return &GetCompArray<T>()->second.at<T>(index);
	}

	template <typename T>
	inline decltype(Chunk::componentDatas_)::iterator Chunk::GetCompArray()
	{
		return componentDatas_.find(static_cast<uint32_t>(ECS::ComponentRegistry::GetIndex<T>()));
	}

	template <typename T>
	inline decltype(Chunk::componentDatas_)::const_iterator Chunk::GetCompArray() const
	{
		return componentDatas_.find(static_cast<uint32_t>(ECS::ComponentRegistry::GetIndex<T>()));
	}

}