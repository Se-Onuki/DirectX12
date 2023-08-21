#pragma once

#include "../Object.h"

class Input;
class ViewProjection;

class PlayerComp : public IComponent {
public:
	using IComponent::IComponent;
	~PlayerComp() override = default;

	void Init() override;
	void Update() override;

	void Attack();

	void SetViewProjection(const ViewProjection *const viewProjection) {
		viewProjection_ = viewProjection;
	}

private:
	Input *input_ = nullptr;

	const ViewProjection *viewProjection_ = nullptr;

	const float bulletSpeed_ = 0.25f;
	const float moveSpeed_ = 0.05f;
};