#include "Entity.h"
#include "Component/ModelComp.h"
#include "Component/Rigidbody.h"


void Entity::Init() {
	isActive_ = true;
}

void Entity::Reset() {
	for (auto &component : componentMap_) {
		component.second->Reset();
	}
}

void Entity::Update(float deltaTime) {
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

void Entity::Draw(const Camera<Render::CameraType::Projecction> &vp) const {
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


const Vector3 &Entity::GetWorldPos() {
	transform_.CalcMatrix();
	return *reinterpret_cast<Vector3 *>(transform_.matWorld_.m[3]);
}

void Entity::OnCollision(Entity *const other) {
	for (auto &component : componentMap_) {
		component.second->OnCollision(other);
	}
}

void Entity::ImGuiWidget() {
	transform_.ImGuiWidget();
	for (auto &component : componentMap_) {
		component.second->ImGuiWidget();
	}
}

IComponent::IComponent(Entity *const object) : object_(object), transform_(&static_cast<BaseTransform &>(object->transform_)) {
}
