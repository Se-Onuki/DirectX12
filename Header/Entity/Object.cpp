#include "Object.h"

IComponent::IComponent(Object *object) : object_(object) {
}

void Object::Init() {
	isActive_ = true;
	transform_.InitResource();
}

void Object::Update() {
	for (auto &component : componentMap_) {
		component.second->Update();
	}
	transform_.UpdateMatrix();
}
void Object::Draw(const ViewProjection &vp) const {
	for (auto &component : componentMap_) {
		component.second->Draw(vp);
	}
}

const Vector3 &Object::GetWorldPos() {
	transform_.CalcMatrix();
	return *reinterpret_cast<Vector3 *>(transform_.matWorld_.m[3]);
}

void Object::OnCollision(Object *const other) {
	for (auto &component : componentMap_) {
		component.second->OnCollision(other);
	}
}

void Object::SetActive(const bool newState) {
	isActive_ = newState;
}
