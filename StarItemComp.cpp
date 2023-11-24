#include "StarItemComp.h"
#include "Header/Entity/Component/ModelComp.h"
#include "Engine/DirectBase/Model/ModelManager.h"

void StarItemComp::Init() {
	auto *const modelComp = object_->AddComponent<ModelComp>();

	modelComp->AddBone("Body", ModelManager::GetInstance()->GetModel("StarItem"));

	transform_->CalcMatrix();
	collider_.centor = transform_->GetGrobalPos();

	collider_.radius = 1.5f;
}

void StarItemComp::Reset() {

}

void StarItemComp::Update() {
	auto *const modelComp = object_->AddComponent<ModelComp>();

	transform_->CalcMatrix();
	collider_.centor = transform_->GetGrobalPos();
	modelComp->GetBone("Body")->transform_->rotate.y = Angle::Mod(modelComp->GetBone("Body")->transform_->rotate.y + 45._deg * object_->GetDeltaTime());

}

void StarItemComp::CollectItem() {
	if (isCollected_) {
		auto *const modelComp = object_->AddComponent<ModelComp>();
		modelComp->GetBone("Body")->transform_->scale = Vector3::zero;

		isCollected_ = false;
	}
}
