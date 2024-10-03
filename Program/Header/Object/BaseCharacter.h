#pragma once
#include "../../Engine/Utils/Math/Transform.h"
#include <string>
#include <unordered_map>
#include "../../Engine/DirectBase/Render/Camera.h"

class Model;

class BaseCharacter {

protected:
	std::unordered_map<std::string, Model *> modelMap_;
	Transform transformOrigin_;

public:
	BaseCharacter() = default;
	virtual ~BaseCharacter() = default;

	virtual void Init(const std::unordered_map<std::string, Model *> &model);

	virtual void Update();

	virtual void Draw(const Camera<Render::CameraType::Projecction> &vp) const;

	const Transform &GetWorldTransform() const { return transformOrigin_; }

private:
};