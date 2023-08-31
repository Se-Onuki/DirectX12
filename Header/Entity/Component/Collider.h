#pragma once

#include "../Object.h"
#include <stdint.h>
#include <list>

enum class CollisionFilter : uint32_t {
	Player = 0b00001,
	Enemy = 0b00010,
	Bullet = 0b00100,
	Item = 0b01000,
	Ground = 0b10000,
};
// ビット演算用
CollisionFilter operator|(CollisionFilter A, CollisionFilter B);
CollisionFilter operator&(CollisionFilter A, CollisionFilter B);

class ColliderComp : public IComponent {
	// 衝突属性(自分自身が持つ属性)
	uint32_t collisionAttribute_ = 0xFFFFFFFF;
	// 衝突マスク(相手)
	uint32_t collisionMask_ = 0xFFFFFFFF;

	// 衝突無効化(相手が特定の属性を持っていた場合衝突を無効化)
	uint32_t collisionCancel_ = 0x0;

protected:



	Vector3 centor_{};
	float radius_ = 0.5f;

public:
	// 親のコンストラクタを使う
	using IComponent::IComponent;

	/// @brief 衝突属性を取得
	/// @return 衝突属性(自分)
	uint32_t GetCollisionAttribute() const { return collisionAttribute_; }
	/// @brief 衝突属性を設定
	/// @param newAttribute 衝突属性(自分)
	void SetCollisionAttribute(const uint32_t newAttribute) { collisionAttribute_ = newAttribute; }
	/// @brief 衝突マスクを取得
	/// @return 衝突マスク(相手)
	uint32_t GetCollisionMask() const { return collisionMask_; }
	/// @brief 衝突マスクを設定
	/// @param newMask 衝突マスク(相手)
	void SetCollisionMask(const uint32_t newMask) { collisionMask_ = newMask; }

	/// @brief 衝突無効化フラグを取得
	/// @return 衝突無効化フラグ
	uint32_t GetCollisionCancel() const { return collisionCancel_; }
	/// @brief 衝突無効化フラグを設定
	/// @param newMask 衝突無効化フラグ
	void SetCollisionCancel(const uint32_t newFlag) { collisionCancel_ = newFlag; }

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