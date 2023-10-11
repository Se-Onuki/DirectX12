#pragma once
#include "BaseCharacter.h"
#include "../Collision/Collision.h"

class Platform : public BaseCharacter {
public:
	Platform() = default;
	~Platform() = default;

	void Init(const std::unordered_map<std::string, Model *> &model) override;
	void Update() override;
	void Draw(const Camera<Render::CameraType::Projecction> &vp) const override;

	void SetPos(const Vector3 &pos) { centor_ = pos; }
	void SetOffset(const Vector3 &offset) { offset_ = offset; }
	void SetIsMove(bool isMove) { isMove_ = isMove; }

	void CalcCollider();

	const OBB &GetCollider() const { return collider_; }

private:

	Transform modelTransform_;

	OBB collider_;

	bool isMove_ = false;

	Vector3 centor_ = {};
	Vector3 offset_ = { 0.f,0.f,20.f };

	uint32_t nowTime_ = 0u;
	uint32_t needTime_ = 360u;

};