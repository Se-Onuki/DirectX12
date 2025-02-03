#include "NT_ServerScene.h"

NT_ServerScene::NT_ServerScene()
{
}

NT_ServerScene::~NT_ServerScene()
{
}

void NT_ServerScene::OnEnter()
{
	input_ = SolEngine::Input::GetInstance();

	if (not InitServer()) {
		SolEngine::SceneManager::GetInstance()->ChangeScene("TitleScene", 0.f);
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
	light_ = std::make_unique<DirectionLight>();
	light_->Init();
	camera_.translation_ = Vector3::front * -10.f;
	camera_.UpdateMatrix();

	thread = std::make_unique<std::thread>(ServerNetworkPosition);
}

void NT_ServerScene::OnExit()
{
	isRunning_ = false;
	thread->join();
}

void NT_ServerScene::Update()
{
	if (not server_) { return; }
	if (isUpdate_) {
		playerB_ = recvPos_;
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

	playerA_ += velocity.Nomalize() * ImGui::GetIO().DeltaTime;

	renderBuffer_[0] = { .transform = Matrix4x4::Affine(Vector3::one, Vector3::zero, playerA_) };
	renderBuffer_[1] = { .transform = Matrix4x4::Affine(Vector3::one, Vector3::zero, playerB_) };
}

void NT_ServerScene::Draw()
{
	if (not server_) { return; }
	DirectXCommon *const dxCommon = DirectXCommon::GetInstance();
	ID3D12GraphicsCommandList *const commandList = dxCommon->GetCommandList();
	Model::StartDraw(commandList);
	Model::SetPipelineType(Model::PipelineType::kShadowParticle);
	light_->SetLight(commandList);
	modelRender_.DrawExecute(camera_);

	Model::EndDraw();
}

IsSuccess NT_ServerScene::InitServer()
{
	server_ = SoLib::TcpServer::Generate(kPort_);
	if (not server_) { return false; }
	return server_->ConnectionWait();
}

void ServerNetworkPosition()
{

	while (NT_ServerScene::isRunning_) {
		auto &client = NT_ServerScene::server_->GetClient();

		auto size = client.Recv(&NT_ServerScene::recvPos_, sizeof(Vector3));

		if (size == static_cast<int32_t>(SOCKET_ERROR)) {
			return;
		}

		client.Send(&NT_ServerScene::playerA_, sizeof(Vector3));
		NT_ServerScene::isUpdate_.store(true);
	}

}
