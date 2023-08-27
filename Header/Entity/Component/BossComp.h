#pragma once
#include "../Object.h"

class BossComp : public IComponent {
public:
	using IComponent::IComponent;
	~BossComp() override = default;

	void Init();

private:

};