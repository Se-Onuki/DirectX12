/// @file NT_ClientScene.h
/// @brief NTのサーバーのシーン
/// @author ONUKI seiya
#pragma once
#include "SceneManager.h"

#include "../Engine/DirectBase/3D/DirectionLight.h"
#include "../Engine/DirectBase/2D/Sprite.h"
#include "../Engine/DirectBase/Render/CameraAnimations/CameraManager.h"

#include "../Engine/DirectBase/Model/Model.h"
#include "../../Engine/Utils/Math/Transform.h"

#include "../Engine/DirectBase/Input/Input.h"
#include "../Engine/DirectBase/Base/Audio.h"
#include "../Header/Object/Particle.h"

#include "../Header/Object/Block/BlockManager.h"
#include "../Header/Object/Ground.h"
#include "../Game/Resource/GameScore.h"
#include "../Header/Object/Fade.h"
#include "../Engine/Utils/Network/TcpClient.h"
#include "../Header/Entity/Entity.h"
#include "../Engine/DirectBase/Render/ModelInstancingRender/ModelInstancingRender.h"

void ClientNetworkPosition();

class NT_ClientScene : public IScene {
public:
	NT_ClientScene();
	~NT_ClientScene();

	void OnEnter() override;
	void OnExit() override;

	void Update() override;
	void Draw() override;

	constexpr static uint16_t kPort_ = 8000;

	constexpr static std::string_view kFileName_ = "./resources/IP/HostIP.txt";

	inline static std::atomic<bool> isUpdate_ = false;
	inline static std::atomic<bool> isRunning_ = true;

	inline static Vector3 recvPos_;

	inline static Vector3 playerA_;
	inline static Vector3 playerB_;

	// サーバー
	inline static std::unique_ptr<TcpClient> client_;
private:

	IsSuccess InitClient();

private:
	// フェード
	Fade *fade_ = nullptr;
	// 入力インスタンス
	Input *input_ = nullptr;
	// 音インスタンス
	Audio *audio_ = nullptr;
	// カメラマネージャー
	CameraManager *cameraManager_ = nullptr;
	std::unique_ptr<std::thread> thread;

	// 平行光源
	std::unique_ptr<DirectionLight> light_ = nullptr;

	Ground ground_{ 0.f, Vector2{100.f,100.f} };

	BlockManager *blockRender_;

	Camera3D camera_;

	SolEngine::ResourceHandle<SolEngine::ModelData> model_;

	/// @briefインスタンシング描画の配列
	SolEngine::ModelInstancingRender modelRender_;

	std::span<Particle::ParticleData> renderBuffer_;

	// bgm
	Audio::SoundHandle soundA_;
};