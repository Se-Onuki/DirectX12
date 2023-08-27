#include "Object.h"
#include "Component/ModelComp.h"


void Object::Init() {
	isActive_ = true;
	transform_.InitResource();
}

void Object::Reset() {
	for (auto &component : componentMap_) {
		component.second->Reset();
	}
}

void Object::Update() {
	auto *const modelComp = GetComponent<ModelComp>();
	for (auto &component : componentMap_) {
		if (modelComp != component.second.get())
			component.second->Update();
	}
	transform_.UpdateMatrix();
	if (modelComp)
		modelComp->Update();
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

void Object::ImGuiWidget() {
	for (auto &component : componentMap_) {
		component.second->ImGuiWidget();
	}
}