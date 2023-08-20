#pragma once

#include "../Math/Transform.h"
#include "../Model/Model.h"

#include "Object.h"
class Input;

class Player : public Object {
public:
	Player() = default;
	~Player() override = default;

	void Init() override;

	void Attack();


private:
	Input *input_ = nullptr;

	const ViewProjection *viewProjection_ = nullptr;

	const float bulletSpeed_ = 0.25;
	const float moveSpeed_ = 0.5f;
};