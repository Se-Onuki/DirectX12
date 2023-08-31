#pragma once
#include "Object.h"

class PlayerBullet : public Object {
public:
	//using Object::Object;
	PlayerBullet() = default;
	//PlayerBullet(PlayerBullet *) {};
	~PlayerBullet() override = default;

	void Init() override;

private:

};
