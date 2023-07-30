#include "DirectBase/Base/LeakChecker.h"

#include <Windows.h>
#include <cstdint>

#include <stdio.h>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <dxgidebug.h>
#include <dxcapi.h>

#include <wrl.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")

#include <string>
#include <format>

#include "DirectBase/Base/WinApp.h"

#include "externals/DirectXTex/DirectXTex.h"

#include "Header/String/String.hpp"
#include "Header/Render/Render.hpp"

#include "Header/Math/Math.hpp"

#include "Header/Math/Vector3.h"
#include "Header/Math/Vector4.h"

#include "Header/Math/Matrix3x3.h"
#include "Header/Math/Matrix4x4.h"

#include "Header/Math/Transform.h"

#include "Header/Texture/Texture.h"
#include "Header/Create/Create.h"
#include "Header/Descriptor/DescriptorHandIe.h"

#include <algorithm>

#include "Header/Model/Model.h"
#include "DirectBase/3D/ViewProjection/ViewProjection.h"
#include "DirectBase/Base/DirectXCommon.h"
#include "DirectBase/Base/TextureManager.h"
#include "DirectBase/Base/Shader.h"
#include "DirectBase/Base/ImGuiManager.h"
#include "DirectBase/3D/DirectionLight.h"
#include "DirectBase/2D/Sprite.h"

#include "Scene/SceneManager.h"
#include "Scene/GameScene.h"

class Object {
public:
	Object(const std::string &filePath) {
		model_.reset(Model::LoadObjFile("", filePath));
		transform_.InitResource();
	}
	~Object() = default;

	std::unique_ptr<Model> model_ = nullptr;
	Transform transform_{};
	void ImGuiWidget() {
		transform_.ImGuiWidget();
		model_->ImGuiWidget();
	}
	void Draw(const ViewProjection &viewProjection)const {
		model_->Draw(transform_, viewProjection);
	}
};

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	WinApp::StaticInit();

	WinApp *const winApp = WinApp::GetInstance();
	winApp->CreateGameWindow("DirectXGame");

	DirectXCommon *const dxCommon = DirectXCommon::GetInstance();
	dxCommon->Init(winApp);

	ID3D12GraphicsCommandList *const commandList = dxCommon->commandList_.Get();

	TextureManager *const textureManager = TextureManager::GetInstance();

	textureManager->Init(dxCommon->GetDevice(), commandList);
	TextureManager::Load("white2x2.png");

	ImGuiManager::StaticInit(winApp->GetHWND(), dxCommon->GetDevice(), (int32_t)dxCommon->backBuffers_.size(), textureManager->GetSRVHeap());


	Shader::StaticInit();
	Model::StaticInit();
	Sprite::StaticInit();

	SceneManager *const sceneManager = SceneManager::GetInstance();
	sceneManager->AddScene("Game", new GameScene);
	sceneManager->ChangeScene("Game");


	ViewProjection viewProjection;
	viewProjection.Init();


	std::unique_ptr<DirectionLight> light{ DirectionLight::Create() };

	std::unique_ptr<Sprite> sprite{ Sprite::Create() };

	std::list<std::unique_ptr<Object>> objectArray_;


	// ウィンドウのxボタンが押されるまでループ
	while (true) {
		if (winApp->ProcessMessage()) break;

#pragma region ImGuiに新規フレームであると伝える
		ImGuiManager::StartFlame();
#pragma endregion

#pragma region ゲームの処理

		sceneManager->Update();

		ImGui::Begin("Camera");
		ImGui::DragFloat3("rotate", &viewProjection.rotation_.x, Angle::Dig2Rad);
		ImGui::DragFloat3("translate", &viewProjection.translation_.x, 0.1f);
		ImGui::End();


		ImGui::Begin("UI");
		sprite->ImGuiWidget();
		ImGui::End();


		ImGui::Begin("Light");
		light->ImGuiWidget();
		ImGui::End();

		ImGui::Begin("ObjectLoader");
		static char fileName[32];
		ImGui::InputText(".obj", fileName, 32u);
		if (ImGui::Button("Load")) {
			objectArray_.emplace_back(new Object(fileName + std::string(".obj")));
		}
		if (ImGui::Button("Add Plane")) {
			objectArray_.emplace_back(new Object("plane.obj"));
		}
		if (ImGui::Button("Add Sphere")) {
			objectArray_.emplace_back(new Object("sphere.obj"));
		}
		if (ImGui::Button("Add Suzanne")) {
			objectArray_.emplace_back(new Object("suzanne.obj"));
		}
		if (ImGui::Button("Add MultiMaterial")) {
			objectArray_.emplace_back(new Object("multiMaterial.obj"));
		}
		if (ImGui::Button("Add Teapot")) {
			objectArray_.emplace_back(new Object("teapot.obj"));
		}
		if (ImGui::Button("Add Bunny(Too Heavy)")) {
			objectArray_.emplace_back(new Object("bunny.obj"));
		}


		std::list<std::unique_ptr<Object>>::iterator it = objectArray_.begin();
		uint32_t index = 0;
		while (it != objectArray_.end()) {

			if (ImGui::TreeNode(((*it)->model_->name_ + "[" + std::to_string(index) + "]").c_str())) {
				(*it)->ImGuiWidget();
				ImGui::TreePop();
			}
			(*it)->transform_.UpdateMatrix();



			if (ImGui::Button(("Delete##" + std::to_string(index)).c_str())) {
				objectArray_.erase(it++); // 削除する前にイテレータをインクリメント
			}
			else {
				++it;
				index++;
			}
		}

		ImGui::End();

		viewProjection.UpdateMatrix();

		ImGui::ShowDemoWindow();

#pragma endregion

#pragma region ImGuiの内部コマンドを生成する
		ImGuiManager::CreateCommand();
#pragma endregion


#pragma region コマンドを積み込んで確定させる

		dxCommon->StartDraw();

		textureManager->StartDraw();

#pragma region コマンドを積む

		sceneManager->Draw();

		Sprite::StartDraw(commandList);
		sprite->Draw();
		Sprite::EndDraw();

		Model::StartDraw(commandList);

		light->SetLight(commandList);

		// モデルの描画

		for (auto &obj : objectArray_) {
			obj->Draw(viewProjection);
		}

		Model::EndDraw();

#pragma endregion

#pragma region ImGuiの描画
		ImGuiManager::Draw(commandList);

#pragma endregion
		dxCommon->EndDraw();

#pragma endregion

	}

#pragma region 各種解放

	ImGuiManager::Finalize();

	dxCommon->Finalize();

	winApp->Finalize();

#pragma endregion

	return 0;
}