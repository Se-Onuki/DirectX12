#include "Collider.h"

void CollisionManager::ChackAllCollision() {

	Collider *colliderA, *colliderB;
	std::list<Collider *>::iterator itrA = colliderList_.begin();
	for (; itrA != colliderList_.end(); ++itrA) {
		colliderA = *itrA;
		std::list<Collider *>::iterator itrB = itrA;
		itrB++;
		for (; itrB != colliderList_.end(); ++itrB) {
			colliderB = *itrB;
			CheckCollisionPair(colliderA, colliderB);
		}
	}
}

void CollisionManager::CheckCollisionPair(Collider *const A, Collider *const B) {
	uint32_t test = (A->GetCollisionAttribute() & B->GetCollisionMask());
	test = test;
	if ((A->GetCollisionAttribute() & B->GetCollisionMask()) == 0u ||
		(B->GetCollisionAttribute() & A->GetCollisionMask()) == 0u) {
		return;
	}

	//const Vector3 aPos = A->object_->GetWorldPos();
	//const Vector3 bPos = B->object_->GetWorldPos();

	//if ((aPos - bPos).Length() <= A->GetRadius() + B->GetRadius()) {
	//	A->OnCollision(B);
	//	B->OnCollision(A);
	//}
}

void Collider::OnCollision(Collider *const other) {
	other;
}

void Collider::Init() {
	//hitFunc_ = [](Collider *) {};
}
