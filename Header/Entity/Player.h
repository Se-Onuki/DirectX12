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

private:
};