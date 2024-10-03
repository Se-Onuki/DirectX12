#include "Ground.h"
#include "../../Engine/DirectBase/Model/ModelManager.h"
#include "../../Engine/Utils/Math/Math.hpp"
#include "../../Engine/Utils/Math/Euler.h"
#include "Block/BlockManager.h"

void Ground::Init() {
	model_ = ModelManager::GetInstance()->GetModel("Plane");
}

Matrix4x4 Ground::CalcMatrix() const {

	Matrix4x4 result{};

	result.m[3][3] = 1.f;
	result.m[2][1] = -1.f;


	// サイズの指定
	result.m[0][0] = radius_.x;
	result.m[1][2] = radius_.y;


	// 高さの変更
	result.m[3][1] = hight_;

	//result = SoLib::Math::Affine({ radius_.x,radius_.y,1.f }, SoLib::Math::MakeQuaternion({ 90._deg,0.f,0.f }), { 0.f,hight_,0.f });

	return result;
}

void Ground::Draw() {
	static auto *const blockManager = BlockManager::GetInstance();

	blockManager->AddBox(model_, IBlock{ .transMat_ = CalcMatrix(), .color_ = 0x555555FF });

}
