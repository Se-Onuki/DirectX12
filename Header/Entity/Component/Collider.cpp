#include "Collider.h"

void CollisionManager::ChackAllCollision() {

	ColliderComp *colliderA, *colliderB;
	std::list<ColliderComp *>::iterator itrA = colliderList_.begin();
	for (; itrA != colliderList_.end(); ++itrA) {
		colliderA = *itrA;
		std::list<ColliderComp *>::iterator itrB = itrA;
		itrB++;
		for (; itrB != colliderList_.end(); ++itrB) {
			colliderB = *itrB;
			CheckCollisionPair(colliderA, colliderB);
		}
	}
}

void CollisionManager::CheckCollisionPair(ColliderComp *const A, ColliderComp *const B) {

	// キャンセル条件
	if ((A->GetCollisionAttribute() & B->GetCollisionCancel()) != 0u ||
		(B->GetCollisionAttribute() & A->GetCollisionCancel()) != 0u) {
		return;
	}
	// ヒット検知
	if ((A->GetCollisionAttribute() & B->GetCollisionMask()) == 0u ||
		(B->GetCollisionAttribute() & A->GetCollisionMask()) == 0u) {
		return;
	}

	const Vector3 aPos = A->GetGlobalCentor();
	const Vector3 bPos = B->GetGlobalCentor();

	if ((aPos - bPos).Length() <= A->GetRadius() + B->GetRadius()) {
		Entity *const aObject = A->object_;
		Entity *const bObject = B->object_;

		aObject->OnCollision(bObject);
		bObject->OnCollision(aObject);
	}
}

Vector3 ColliderComp::GetGlobalCentor() const {
	object_->transform_.CalcMatrix();
	return centor_ * object_->transform_.matWorld_;
}

CollisionFilter operator|(CollisionFilter A, CollisionFilter B) {
	return static_cast<CollisionFilter>(static_cast<uint32_t>(A) | static_cast<uint32_t>(B));
}

CollisionFilter operator&(CollisionFilter A, CollisionFilter B) {
	return static_cast<CollisionFilter>(static_cast<uint32_t>(A) & static_cast<uint32_t>(B));
}