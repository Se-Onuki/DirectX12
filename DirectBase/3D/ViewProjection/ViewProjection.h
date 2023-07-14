#pragma once
#include "../../../Header/Math/Matrix4x4.h"
#include "../../../Header/Math/Vector3.h"

class ViewProjection
{
public:
	struct ViewProjectionMatrix {
		Matrix4x4 view;
		Matrix4x4 projection;
		// Vector3 camerapos;
	};

	ViewProjection();
	~ViewProjection();


};