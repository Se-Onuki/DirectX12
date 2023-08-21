#pragma once

#include "../Object.h"
#include "../../Math/Vector3.h"

class Rigidbody : public IComponent {
public:
	using IComponent::IComponent;
	~Rigidbody() = default;

	void Init();
	void Update();

	void AddAcceleration(const Vector3 &acceleration) {
		acceleration_ = acceleration;
	}

	const Vector3 &GetVelocity() const {
		return velocity_;
	}

	const Vector3 &GetAcceleration() const {
		return acceleration_;
	}

private:
	Vector3 velocity_;
	Vector3 acceleration_;

};