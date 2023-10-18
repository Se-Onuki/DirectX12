#include "Platform.h"
#include "../../Engine/DirectBase/Model/Model.h"

void Platform::Init(const std::unordered_map<std::string, Model *> &model) {
	BaseCharacter::Init(model);

	transformOrigin_.translate = centor_;

	transformOrigin_.CalcMatrix();

	const Vector3 radius = { 10.f,0.1f,10.f };

	collider_.size = radius;

	modelTransform_.SetParent(transformOrigin_);
	modelTransform_.scale = radius;
	modelTransform_.CalcMatrix();

	CalcCollider();
}

void Platform::Update(const float deltaTime) {

	// 動作フラグが有効ならば
	if (rotSpeed_.Length() >= 0.f) {

		Vector3 angle = rotSpeed_ * deltaTime * Angle::Dig360;
		transformOrigin_.rotate = Angle::Mod(transformOrigin_.rotate + angle);
	}
	Vector3 diff = offset_ * Matrix4x4::EulerRotate(transformOrigin_.rotate);

	transformOrigin_.translate = centor_ + diff;


	transformOrigin_.CalcMatrix();

	collider_.SetMatrix(transformOrigin_.matWorld_);
	modelTransform_.UpdateMatrix();
	CalcCollider();
}

void Platform::Draw(const Camera<Render::CameraType::Projecction> &camera) const {
	for (auto &model : modelMap_) {
		model.second->Draw(modelTransform_, camera);
	}
}

void Platform::CalcCollider() {
	collider_.SetMatrix(transformOrigin_.matWorld_);
}
