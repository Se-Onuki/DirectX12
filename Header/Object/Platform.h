#pragma once
#include "BaseCharacter.h"
#include "../Collision/Collision.h"

class Platform : public BaseCharacter {
public:
	Platform() = default;
	~Platform() = default;

	void Init(const std::unordered_map<std::string, Model *> &model) override;
	void Update(const float deltaTime) override;
	void Draw(const Camera<Render::CameraType::Projecction> &vp) const override;

	void SetPos(const Vector3 &pos) { centor_ = pos; }
	void SetOffset(const Vector3 &offset) { offset_ = offset; }
	void SetRotSpeed(const Vector3 &speed) { rotSpeed_ = speed; }

	void CalcCollider();

	const OBB &GetCollider() const { return collider_; }

private:

	Transform modelTransform_;

	OBB collider_;


	Vector3 centor_ = {};
	Vector3 offset_ = { 0.f,0.f,0.f };
	Vector3 rotSpeed_ = { 0.f,0.f,0.f };

	float nowTime_ = 0.f;

};