#pragma once
#include "Header/Entity/Entity.h"

class StarItemComp : public IComponent {
public:
	using IComponent::IComponent;
	~StarItemComp() = default;

	void Init() override;
	void Update() override;

private:

};