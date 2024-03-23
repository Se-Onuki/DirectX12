#include "Component.hpp"
//#include "Component.hpp"
////
////
////Component::Component() {}
////
////Component::~Component() {}
////
////
////
////Transform::Transform() {}
////
////Transform::~Transform() {}

SoLib::Color::RGB4 ECS::ColorLarp::EaseColor(float t) {

	return SoLib::Lerp(start_, end_, easeFunc_(t));
}

void  ECS::FollowCamera::TransferData(Camera3D &camera, const Vector3 &translate) const {

	camera.rotation_ = this->rotation_;
	camera.translation_ = translate + (offset_ * rotation_);
	camera.fovAngleY = this->fovAngleY;
	camera.aspectRatio = this->aspectRatio;
	camera.nearZ = this->nearZ;
	camera.farZ = this->farZ;

}
