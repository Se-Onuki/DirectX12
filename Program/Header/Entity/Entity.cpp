#include "Entity.h"
#include "Component/ModelComp.h"
#include "Component/Rigidbody.h"


void GameObject::Init() {
	isActive_ = true;
}

void GameObject::Reset() {
	for (auto &component : componentMap_) {
		component.second->Reset();
	}
}

void GameObject::Update(float deltaTime) {
	deltaTime_ = deltaTime;

	auto *const modelComp = GetComponent<ModelComp>();
	auto *const rigidbody = GetComponent<Rigidbody>();

	if (rigidbody) {
		rigidbody->Update();
	}
	for (auto &component : componentMap_) {
		if (modelComp != component.second.get() && rigidbody != component.second.get()) {
			component.second->Update();
		}
	}
	transform_.UpdateMatrix();
	if (modelComp) {
		modelComp->Update();
	}
}

void GameObject::Draw(const Camera3D &vp) const {
	auto *const modelComp = GetComponent<ModelComp>();

	for (auto &component : componentMap_) {
		if (modelComp != component.second.get()) {
			component.second->Draw(vp);
		}
	}
	if (modelComp) {
		modelComp->Draw(vp);
	}
}


const Vector3 &GameObject::GetWorldPos() {
	transform_.CalcMatrix();
	return *reinterpret_cast<Vector3 *>(transform_.matWorld_.m[3].data());
}

void GameObject::OnCollision(GameObject *const other) {
	for (auto &component : componentMap_) {
		component.second->OnCollision(other);
	}
}

void GameObject::ImGuiWidget() {
	transform_.ImGuiWidget();
	for (auto &component : componentMap_) {
		component.second->ImGuiWidget();
	}
}

IComponent::IComponent(GameObject *const object) : object_(object), transform_(&static_cast<BaseTransform &>(object->transform_)) {
}

float IComponent::GetDeltaTime() const {
	return monoTimeScale_ * object_->GetDeltaTime();
}
