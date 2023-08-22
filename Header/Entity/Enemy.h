#pragma once
#include "Object.h"

class Enemy : public Object {
public:
	Enemy() = default;
	~Enemy() override = default;

	void Init() override;

	void OnCollision(Object *const other) override;
private:

	uint32_t hp_ = 3u;
};