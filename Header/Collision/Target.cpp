#include "Target.h"

#include "../Entity/Component/Collider.h"
#include "../Render/Render.hpp"
#include "../../DirectBase/3D/ViewProjection/ViewProjection.h"
#include "../../DirectBase/Base/WinApp.h"

void Targeting::clear() {
	colliderList_.clear();
	hitCollider_ = nullptr;
}

void Targeting::push_back(Object *object) {
	auto *const colliderComp = object->GetComponent<ColliderComp>();
	if (colliderComp) {
		colliderList_.push_back(colliderComp);
	}
}

void Targeting::Update(const ViewProjection &vp) {
	for (auto &collider : colliderList_) {
		const Vector3 &colliderPos = collider->GetWorldCentor();
		const static Matrix4x4 matViewport =
			Render::MakeViewportMatrix({ 0.f,0.f }, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);

		const Matrix4x4 &matVPVp = vp.matView_ * vp.matProjection_ * matViewport;

		const Vector2 &pos2D = Render::WorldToScreen(colliderPos, matVPVp).ToVec2();

		if ((reticlePos_ - pos2D).Length() <= lineRadius_) {
			hitCollider_ = collider;
			return;
		}
		/*const Vector3 &closestPoint = mouseSegment_.ClosestPoint(colliderPos);
		if ((closestPoint - colliderPos).Length() <= collider->GetRadius() + lineRadius_) {
			hitCollider_ = collider;
			return;
		}*/
	}
}