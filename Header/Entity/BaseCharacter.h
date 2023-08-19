#pragma once
#include "../Math/Transform.h"
#include <string>
#include <unordered_map>

class Model;
class ViewProjection;

class BaseCharacter {

protected:
	std::unordered_map<std::string, Model *> modelMap_;
	Transform transformOrigin_;

public:
	BaseCharacter() = default;
	virtual ~BaseCharacter() = default;

	virtual void Init(const std::unordered_map<std::string, Model *> &model);

	virtual void Update();

	virtual void Draw(const ViewProjection &vp) const;

	const Transform &GetTransform() const {
		return transformOrigin_;
	}

private:
};