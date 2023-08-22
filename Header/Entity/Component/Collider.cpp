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
	/*uint32_t test = (A->GetCollisionAttribute() & B->GetCollisionMask());
	test = test;*/
	if ((A->GetCollisionAttribute() & B->GetCollisionMask()) == 0u ||
		(B->GetCollisionAttribute() & A->GetCollisionMask()) == 0u) {
		return;
	}

	const Vector3 aPos = A->GetWorldCentor();
	const Vector3 bPos = B->GetWorldCentor();

	if ((aPos - bPos).Length() <= A->GetRadius() + B->GetRadius()) {
		Object *const aObject = A->object_;
		Object *const bObject = B->object_;

		aObject->OnCollision(bObject);
		bObject->OnCollision(aObject);
	}
}

Vector3 ColliderComp::GetWorldCentor() const {
	object_->transform_.CalcMatrix();
	return centor_ * object_->transform_.matWorld_;
}
