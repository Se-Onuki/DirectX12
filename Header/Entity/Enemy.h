#pragma once
#include "Object.h"

class Enemy : public Object {
public:
	Enemy() = default;
	~Enemy() override = default;

	void Init() override;

private:
};