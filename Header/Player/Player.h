#pragma once

#include "../Math/Transform.h"
#include "../Model/Model.h"
#include "../Entity/BaseCharacter.h"
class Input;

class Player : public BaseCharacter {
public:
	Player() = default;
	~Player() override = default;

	void Init(const std::unordered_map<std::string, Model *> &model) override;

	void Update() override;

	void Draw(const ViewProjection &vp) const override;

	void SetViewProjection(const ViewProjection *const viewProjection) {
		viewProjection_ = viewProjection;
	}

private:
	Input *input_ = nullptr;

	const float moveSpeed_ = 0.5f;
	const ViewProjection *viewProjection_ = nullptr;
};