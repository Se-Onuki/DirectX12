#pragma once

#include "../Object.h"
#include <stdint.h>
#include <list>

#include <functional>


enum class CollisionFilter {
	Player = 0b00001, Enemy = 0b00010
};


class ColliderComp : public IComponent {
	// 衝突属性(自分)
	uint32_t collisionAttribute_ = 0xFFFFFFFF;
	// 衝突マスク(相手)
	uint32_t collisionMask_ = 0xFFFFFFFF;

protected:

	float radius = 0.5f;

public:
	// 親のコンストラクタを使う
	using IComponent::IComponent;

	/// @brief 衝突属性を取得
	/// @return 衝突属性(自分)
	const uint32_t GetCollisionAttribute() const {
		return collisionAttribute_;
	}
	/// @brief 衝突属性を設定
	/// @param newAttribute 衝突属性(自分)
	void SetCollisionAttribute(const uint32_t newAttribute) {
		collisionAttribute_ = newAttribute;
	}
	/// @brief 衝突マスクを取得
	/// @return 衝突マスク(相手)
	const uint32_t GetCollisionMask() const {
		return collisionMask_;
	}
	/// @brief 衝突マスクを設定
	/// @param newMask 衝突マスク(相手)
	void SetCollisionMask(const uint32_t newMask) {
		collisionMask_ = newMask;
	}

	const float &GetRadius() const {
		return radius;
	}
	void SetRadius(const float &rad) {
		radius = rad;
	}

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