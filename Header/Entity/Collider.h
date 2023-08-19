#pragma once

#include "Object.h"
#include <stdint.h>
#include <list>

#include <functional>

class Collider /*: public IComponent*/ {
	// 衝突属性(自分)
	uint32_t collisionAttribute_ = 0xFFFFFFFF;
	// 衝突マスク(相手)
	uint32_t collisionMask_ = 0xFFFFFFFF;

	//std::function<void(Collider *const)> hitFunc_;

protected:

	float radius = 0.5f;

public:
	/// @brief 衝突属性を取得
	/// @return 衝突属性
	const uint32_t GetCollisionAttribute() const {
		return collisionAttribute_;
	}
	/// @brief 衝突属性を設定
	/// @param newAttribute 衝突属性
	void SetCollisionAttribute(const uint32_t newAttribute) {
		collisionAttribute_ = newAttribute;
	}
	/// @brief 衝突マスクを取得
	/// @return 衝突マスク
	const uint32_t GetCollisionMask() const {
		return collisionMask_;
	}
	/// @brief 衝突マスクを設定
	/// @param newMask 衝突マスク
	void SetCollisionMask(const uint32_t newMask) {
		collisionMask_ = newMask;
	}

	const float &GetRadius() const {
		return radius;
	}
	void SetRadius(const float &rad) {
		radius = rad;
	}

	// 当たった時に実行される関数を定義
	void SetFunction(const std::function<void(Collider *const)> &function) {
		function;
		//hitFunc_ = function;
	}

	void Init();

	void OnCollision(Collider *const other);
};



class CollisionManager {
	std::list<Collider *> colliderList_;

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
	void push_back(Collider *collider) {
		colliderList_.push_back(collider);
	}

	void ChackAllCollision();
	void CheckCollisionPair(Collider *const A, Collider *const B);
};