#include "Projectile.h"
#include "HealthComp.h"

void ProjectileComp::OnCollision(Object *other) {
	HealthComp *const healthComp = other->GetComponent<HealthComp>();
	if (healthComp) { healthComp->AddHealth(-damage_); }
	if (isHitDestroy_) { object_->SetActive(false); }
}
