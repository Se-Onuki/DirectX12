#include "NT_ClientScene.h"
#include "../Engine/Utils/IO/File.h"

NT_ClientScene::NT_ClientScene()
{
}

NT_ClientScene::~NT_ClientScene()
{
}

void NT_ClientScene::OnEnter()
{
	input_ = Input::GetInstance();
	if (not InitClient()) {
		SceneManager::GetInstance()->ChangeScene("TitleScene", 0.f);
		return;
	}
	isRunning_ = true;

	auto *const pAssimpManager = SolEngine::ResourceObjectManager<SolEngine::AssimpData>::GetInstance();
	auto *const pModelManager = SolEngine::ResourceObjectManager<SolEngine::ModelData>::GetInstance();

	auto assimpHandle = pAssimpManager->Load({ "","box.obj" });

	model_ = pModelManager->Load({ assimpHandle });
	modelRender_.Init(2u);
	modelRender_.SetModelData(model_);

	renderBuffer_ = modelRender_.Reservation(2u);

	playerA_ = Vector3::right * -2.f;
	playerB_ = Vector3::right * 2.f;

	thread = std::make_unique<std::thread>(ClientNetworkPosition);

	light_ = std::make_unique<DirectionLight>();
	light_->Init();

	camera_.translation_ = Vector3::front * -10.f;
	camera_.UpdateMatrix();
}

void NT_ClientScene::OnExit()
{
	isRunning_.store(false);
	thread->join();
}

void NT_ClientScene::Update()
{
	if (not client_) { return; }
	if (isUpdate_) {
		playerA_ = recvPos_;
		isUpdate_.store(false);
	}
	auto dInput = input_->GetDirectInput();

	Vector3 velocity = {};

	if (dInput->IsPress(DIK_A)) {
		velocity.x--;
	}
	if (dInput->IsPress(DIK_D)) {
		velocity.x++;
	}

	playerB_ += velocity.Nomalize() * ImGui::GetIO().DeltaTime;

	renderBuffer_[0] = { .transform = Matrix4x4::Affine(Vector3::one, Vector3::zero, playerA_) };
	renderBuffer_[1] = { .transform = Matrix4x4::Affine(Vector3::one, Vector3::zero, playerB_) };
}

void NT_ClientScene::Draw()
{
	if (not client_) { return; }
	DirectXCommon *const dxCommon = DirectXCommon::GetInstance();
	ID3D12GraphicsCommandList *const commandList = dxCommon->GetCommandList();
	Model::StartDraw(commandList);
	Model::SetPipelineType(Model::PipelineType::kShadowParticle);
	light_->SetLight(commandList);
	modelRender_.DrawExecute(camera_);

	Model::EndDraw();
}

IsSuccess NT_ClientScene::InitClient()
{
	SoLib::IO::File file(kFileName_);
	std::string hostIP;
	static_cast<std::stringstream &>(file) >> hostIP;
	return (client_ = TcpClient::Generate(hostIP.c_str(), kPort_)) != nullptr;
}

void ClientNetworkPosition()
{
	while (NT_ClientScene::isRunning_) {
		auto &server = NT_ClientScene::client_->GetSocket();

		server.Send(&NT_ClientScene::playerB_, sizeof(Vector3));

		auto size = server.Recv(&NT_ClientScene::recvPos_, sizeof(Vector3));

		if (size == static_cast<int32_t>(SOCKET_ERROR)) {
			return;
		}
		NT_ClientScene::isUpdate_.store(true);
	}

}
