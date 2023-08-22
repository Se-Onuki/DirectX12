#pragma once

#include "../Object.h"
#include "../../Math/Vector3.h"

class Rigidbody : public IComponent {
public:
	using IComponent::IComponent;
	~Rigidbody() = default;

	void Init();
	void Update();

	void AddAcceleration(const Vector3 &vec) { acceleration_ += vec; }
	void SetAcceleration(const Vector3 &vec) { acceleration_ = vec; }
	const Vector3 &GetAcceleration() const { return acceleration_; }

	void SetVelocity(const Vector3 &vec) { velocity_ = vec; }

	const Vector3 &GetVelocity() const { return velocity_; }

	bool GetIsGround() const { return isGround_; }
private:
	Vector3 velocity_{};
	Vector3 acceleration_{};

	bool isGround_ = false;

	static const float deltaTime_;
};