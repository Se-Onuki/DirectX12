#pragma once

#include "BaseCharacter.h"

class Enemy : public BaseCharacter {
public:
	Enemy() = default;
	~Enemy() override;

	void Init(const std::unordered_map<std::string, Model *> &model) override;
	void Update() override;
	void Draw(const Camera<Render::CameraType::Projecction> &camera) const override;

private:
};
