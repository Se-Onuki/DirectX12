#pragma once
#include <typeindex>
#include <stdint.h>
#include <list>
#include "Entity/Entity.hpp"
#include "../Utils/SoLib/SoLib.h"
#include <unordered_set>
#include <set>
#include "ClassData.h"
#include "Component/ComponentRegistry.h"

class Archetype {
public:

	ECS::ComponentRegistry::ComponentFlag compFlag_;

	static constexpr size_t kOneChunkCapacity = 16u * 1024u;

	Archetype() = default;

	void AddClassData(const std::initializer_list<uint32_t> &component) {
		ECS::ComponentRegistry *const compReg = ECS::ComponentRegistry::GetInstance();
		// サイズを追加する
		for (uint32_t arg : component) {
			if (not compFlag_.Get().test(arg)) { totalSize_ += compReg->typeDatas_[arg].typeSize_; }
		}
		compFlag_.AddComp(component);

		// 追加した型で容量を計算する
		chunkCapacity_ = CalcCapacity();
	}

	template<SoLib::IsContainsType<uint32_t> T>
	void AddClassData(const T &component) {
		ECS::ComponentRegistry *const compReg = ECS::ComponentRegistry::GetInstance();
		// サイズを追加する
		for (uint32_t arg : component) {
			if (not compFlag_.Get().test(arg)) { totalSize_ += compReg->typeDatas_[arg].typeSize_; }
		}
		compFlag_.AddComp(component);

		// 追加した型で容量を計算する
		chunkCapacity_ = CalcCapacity();
	}

	template<typename T, typename... TComps>
	void AddClassData() {

		// サイズを追加する
		if (not compFlag_.IsHasComp<T>()) { totalSize_ += sizeof(T); };
		(([this]() { if (not compFlag_.IsHasComp<TComps>()) { totalSize_ += sizeof(TComps); }}), ...);

		compFlag_.AddComp<T, TComps...>();

		// 追加した型で容量を計算する
		chunkCapacity_ = CalcCapacity();
	}
	bool operator==(const Archetype &other) const { return other.compFlag_ == compFlag_; }

	bool operator<=(const Archetype &other) const {
		return (compFlag_.Get() & other.compFlag_.Get()) == compFlag_.Get();
	}

	uint32_t GetTotalSize() const { return totalSize_; }
	uint32_t GetChunkCapacity() const { return chunkCapacity_; }

private:

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
			return obj.compFlag_.Get().to_ullong();
		}
	};
}