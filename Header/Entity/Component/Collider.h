#pragma once

#include "../Object.h"
#include <stdint.h>
#include <list>

enum class CollisionFilter : uint32_t {
	Player = 0b00001,
	Enemy = 0b00010,
	Item = 0b00100,
	Ground = 0b01000,
};
// ビット演算用
CollisionFilter operator|(CollisionFilter A, CollisionFilter B);
CollisionFilter operator&(CollisionFilter A, CollisionFilter B);

class ColliderComp : public IComponent {
	// 衝突属性(自分自身が持つ属性)
	uint32_t collisionAttribute_ = 0xFFFFFFFF;
	// 衝突マスク(相手)
	uint32_t collisionMask_ = 0xFFFFFFFF;

protected:



	Vector3 centor_{};
	float radius_ = 0.5f;

public:
	// 親のコンストラクタを使う
	using IComponent::IComponent;

	/// @brief 衝突属性を取得
	/// @return 衝突属性(自分)
	const uint32_t GetCollisionAttribute() const { return collisionAttribute_; }
	/// @brief 衝突属性を設定
	/// @param newAttribute 衝突属性(自分)
	void SetCollisionAttribute(const uint32_t newAttribute) { collisionAttribute_ = newAttribute; }
	/// @brief 衝突マスクを取得
	/// @return 衝突マスク(相手)
	const uint32_t GetCollisionMask() const { return collisionMask_; }
	/// @brief 衝突マスクを設定
	/// @param newMask 衝突マスク(相手)
	void SetCollisionMask(const uint32_t newMask) { collisionMask_ = newMask; }

	const float &GetRadius() const { return radius_; }
	void SetRadius(const float &rad) { radius_ = rad; }

	const Vector3 &GetCentor() const { return centor_; }
	Vector3 GetGlobalCentor() const;
	void SetCentor(const Vector3 &centor) { centor_ = centor; }

	/// @brief jsonからの読み込み
	/// @param groupName グループ名
	void ApplyVariables(const char *const groupName) override { groupName; }

	/// @brief jsonへの紐づけ
	/// @param groupName グループ名
	void AddVariable(const char *const groupName) const override { groupName; }

};

class CollisionManager {
	std::list<ColliderComp *> colliderList_;

	CollisionManager() = default;
	CollisionManager(const CollisionManager &) = delete;
	CollisionManager operator=(const CollisionManager &) = delete;
	~CollisionManager() = default;

public:
	static CollisionManager *GetInstance() {
		static CollisionManager collisionManager;
		return &collisionManager;
	}
	void clear() {
		colliderList_.clear();
	}

	void push_back(ColliderComp *collider) { colliderList_.push_back(collider); }

	void push_back(Object *object) {
		auto *const colliderComp = object->GetComponent<ColliderComp>();
		if (colliderComp) {
			colliderList_.push_back(colliderComp);
		}
	}

	void ChackAllCollision();
	void CheckCollisionPair(ColliderComp *const A, ColliderComp *const B);
};