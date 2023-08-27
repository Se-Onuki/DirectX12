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
	//const Vector3 &direction = TransformNormal(Vector3::front(), vp.matView_);

	for (auto &collider : colliderList_) {
		const Vector3 &colliderPos = collider->GetWorldCentor();

		const static Matrix4x4 matViewport =
			Render::MakeViewportMatrix({ 0.f,0.f }, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);

		const Matrix4x4 &matVPVp = vp.matView_ * vp.matProjection_ * matViewport;

		const Vector3 &screenPos = Render::WorldToScreen(colliderPos, matVPVp);
		if (screenPos.z > 1.f) {
			continue;
		}
		if ((reticlePos_ - screenPos.ToVec2()).Length() <= radius_) {
			hitCollider_ = collider;
			return;
		}
	}
}