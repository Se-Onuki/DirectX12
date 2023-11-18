#include "StarParticle.h"

// 静的なメンバ変数の実体を宣言
float StarParticle::testParam_ = 0.0f;

void StarParticle::AddItem()
{
	// 調整項目クラスに値を追加
	GlobalVariables::GetInstance()->AddValue(typeid(StarParticle).name(), "test", testParam_);
}

void StarParticle::SetItem()
{
	// 調整項目クラスに値を追加
	GlobalVariables::GetInstance()->SetValue(typeid(StarParticle).name(), "test", testParam_);
}

void StarParticle::ApplyItem()
{
	// 調整項目クラスに値を追加
	testParam_ = GlobalVariables::GetInstance()->Get<float>(typeid(StarParticle).name(), "test");
}

void StarParticle::DisplayImGui()
{
	ImGui::Begin("ParticleParameters", nullptr, ImGuiWindowFlags_MenuBar);
	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu(typeid(StarParticle).name())) {
			ImGui::Text("UniqueParameter");

			if(ImGui::Button("SetUniqueItem")) {
				AddItem();
				SetItem();
			}
			if (ImGui::Button("ApplyUniqueItem")) {
				ApplyItem();
			}

			ImGui::DragFloat("test", &testParam_, 0.01f);
			ImGui::EndMenu();
		}
		
	}
	ImGui::EndMenuBar();
	ImGui::End();
}

void StarParticle::Init() {
	// 回転角のリセット
	transform_.rotate = { 0.0f, transform_.rotate.y, 0.0f };

	// エミッタの角度から放出方向を求める
	const Matrix4x4& mat = Matrix4x4::EulerRotate(Matrix4x4::Yaw, transform_.rotate.y); // エミッタのy軸角度から回転行列を生成
	move_ = TransformNormal(velocity_, mat);											// ベクトルを求める
}

void StarParticle::Update([[maybe_unused]] float deltaTime)
{

	if (!timer_.IsFinish()) {
		transform_.rotate.y += 0.01f;
		transform_.rotate.y = Angle::Mod(transform_.rotate.y);
		transform_.translate += move_;
	}
	else {
		isAlive_ = false;
	}

	// トランスフォームの計算と転送
	transform_.CalcMatrix();
	// タイマー更新
	timer_.Update(deltaTime);
}
