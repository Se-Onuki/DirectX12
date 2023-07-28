#include "Sprite.h"
#include "../Base/TextureManager.h"

Matrix4x4 Sprite::matProjection_{};
ID3D12Device *Sprite::device_{};

//Sprite::Sprite()
//{
//}
//
//Sprite::~Sprite()
//{
//}

void Sprite::StaticInit(ID3D12Device *device) {
	device_ = device;
}

void Sprite::Init() {
	textureHaundle_ = TextureManager::Load("white2x2.png");
}

void Sprite::Draw() {
}

void Sprite::CreateVertex()
{
}
