#pragma once
#include <typeindex>
#include <stdint.h>
#include <list>
#include "../Utils/SoLib/SoLib.h"
#include <unordered_set>
#include <set>
#include "Component/ComponentRegistry.h"

class Archetype {
public:

	/// @brief 必須コンポーネント
	ECS::ComponentRegistry::ComponentFlag required_;

	static constexpr size_t kOneChunkCapacity = 16u * 1024u;

	Archetype() = default;

	/// @brief コンポーネントを追加
	/// @param component 追加するコンポーネント
	void AddClassData(const std::initializer_list<uint32_t> &component) {
		ECS::ComponentRegistry *const compReg = ECS::ComponentRegistry::GetInstance();
		// サイズを追加する
		for (uint32_t arg : component) {
			if (not required_.Get().test(arg)) { totalSize_ += compReg->typeDatas_[arg].typeSize_; }
		}
		required_.AddComp(component);

		// 追加した型で容量を計算する
		chunkCapacity_ = CalcCapacity();
	}

	/// @brief コンポーネントを追加
	/// @param component 追加するコンポーネント
	template<SoLib::IsContainsType<uint32_t> T>
	void AddClassData(const T &component) {
		ECS::ComponentRegistry *const compReg = ECS::ComponentRegistry::GetInstance();
		// サイズを追加する
		for (uint32_t arg : component) {
			if (not required_.Get().test(arg)) { totalSize_ += compReg->typeDatas_[arg].typeSize_; }
		}
		required_.AddComp(component);

		// 追加した型で容量を計算する
		chunkCapacity_ = CalcCapacity();
	}

	/// @brief コンポーネントを追加
	template<typename T, typename... TComps>
	void AddClassData() {

		// サイズを追加する
		if (not required_.IsHasComp<T>()) { totalSize_ += sizeof(T); };
		(([this]() { if (not required_.IsHasComp<TComps>()) { totalSize_ += sizeof(TComps); }})(), ...);	// 生成だけして実行してなかった

		required_.AddComp<T, TComps...>();

		// 追加した型で容量を計算する
		chunkCapacity_ = CalcCapacity();
	}
	bool operator==(const Archetype &other) const { return other.required_ == required_; }

	bool operator<=(const Archetype &other) const {
		return (required_.Get() & other.required_.Get()) == required_.Get();
	}
	/// @brief すべてのコンポーネントを合算したサイズを取得
	uint32_t GetTotalSize() const { return totalSize_; }
	/// @brief 1つのチャンクにいくつ置けるかを取得
	uint32_t GetChunkCapacity() const { return chunkCapacity_; }

private:

	/// @brief 1つのチャンクにいくつ置けるかを計算
	uint32_t CalcCapacity() const { return kOneChunkCapacity / totalSize_; }

	// 最大のサイズ
	uint32_t totalSize_ = 0u;
	// 1つのチャンクにいくつ置けるか
	uint32_t chunkCapacity_ = 0u;

	/// @brief 型の設定
	/// @tparam ...TComps 設定する型
	template<typename... TComps>
	void InnerSetClassData()
	{
		// Entityと型のメモリを合算する
		totalSize_ = (sizeof(TComps) + ...);
		// 1つのチャンクにいくつ置けるかを計算する
		chunkCapacity_ = kOneChunkCapacity / totalSize_;
	}
};



namespace std {
	template<>
	struct hash<Archetype> {
		std::size_t operator()(const Archetype &obj) const {
			return obj.required_.Get().to_ullong();
		}
	};
}