#pragma once
#include "../../../../Engine/DirectBase/Render/Camera.h"

class IPlayerState {
public:
	IPlayerState() = default;
	virtual ~IPlayerState() = default;

	virtual void Init();

	virtual void Update(float deltaTime);

	virtual void Draw(const Camera3D &camera) const;

private:

};