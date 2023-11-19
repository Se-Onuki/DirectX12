#include "StarParticle.h"

// 静的なメンバ変数の実体を宣言
float StarParticle::rotationAmount_ = 0.025f;
float StarParticle::minSize_ = 0.5f;
float StarParticle::maxSize_ = 0.5f;
float StarParticle::dispalyParticleTime_ = 0.05f;

void StarParticle::AddItem()
{
	// 調整項目クラスに値を追加
	GlobalVariables::GetInstance()->AddValue(typeid(StarParticle).name(), "RotationAmount", rotationAmount_);
	GlobalVariables::GetInstance()->AddValue(typeid(StarParticle).name(), "MinSize", minSize_);
	GlobalVariables::GetInstance()->AddValue(typeid(StarParticle).name(), "MaxSize", maxSize_);
	GlobalVariables::GetInstance()->AddValue(typeid(StarParticle).name(), "DispalyParticleTime", dispalyParticleTime_);
}

void StarParticle::SetItem()
{
	// 調整項目クラスに値を追加
	GlobalVariables::GetInstance()->SetValue(typeid(StarParticle).name(), "RotationAmount", rotationAmount_);
	GlobalVariables::GetInstance()->SetValue(typeid(StarParticle).name(), "MinSize", minSize_);
	GlobalVariables::GetInstance()->SetValue(typeid(StarParticle).name(), "MaxSize", maxSize_);
	GlobalVariables::GetInstance()->SetValue(typeid(StarParticle).name(), "DispalyParticleTime", dispalyParticleTime_);
}

void StarParticle::ApplyItem()
{
	// 調整項目クラスから値を取得
	rotationAmount_ = GlobalVariables::GetInstance()->Get<float>(typeid(StarParticle).name(), "RotationAmount");
	minSize_ = GlobalVariables::GetInstance()->Get<float>(typeid(StarParticle).name(), "MinSize");
	maxSize_ = GlobalVariables::GetInstance()->Get<float>(typeid(StarParticle).name(), "MaxSize");
	dispalyParticleTime_ = GlobalVariables::GetInstance()->Get<float>(typeid(StarParticle).name(), "DispalyParticleTime");
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

			ImGui::DragFloat("RotationAmount", &rotationAmount_, 0.001f, 0.0001f, 0.5f);
			ImGui::DragFloat("MinSize", &minSize_, 0.1f, 0.1f, 2.0f);
			ImGui::DragFloat("MaxSize", &maxSize_, 0.001f, 0.1f, 2.0f);
			ImGui::DragFloat("DispalyParticleTime", &dispalyParticleTime_, 0.01f, 0.01f, 2.0f);
			ImGui::EndMenu();
		}
		
	}
	ImGui::EndMenuBar();
	ImGui::End();
}

void StarParticle::Init() {
	// 大きさの設定
	transform_.scale.x = Random::GetRandom<float>(minSize_, maxSize_);
	transform_.scale = { transform_.scale.x , transform_.scale.x , transform_.scale.x };
	// 大きさ取得
	startScale_ = transform_.scale;

	// 回転角のリセット
	transform_.rotate = { 0.0f, transform_.rotate.y, 0.0f };

	// 座標の設定
	float blur = Random::GetRandom<float>(-randomNumber_, randomNumber_);
	transform_.translate.x += blur;
	blur = Random::GetRandom<float>(-randomNumber_, randomNumber_);
	transform_.translate.z += blur;

	// 粒子表示までのタイマー設定
	diplayParticleTimer_.Start(dispalyParticleTime_);
	color_ = { 1.0f, 1.0f, 1.0f, 1.0f };

	// エミッタの角度から放出方向を求める
	const Matrix4x4& mat = Matrix4x4::EulerRotate(Matrix4x4::Yaw, transform_.rotate.y); // エミッタのy軸角度から回転行列を生成
	move_ = TransformNormal(velocity_, mat);											// ベクトルを求める
}

void StarParticle::Update([[maybe_unused]] float deltaTime)
{
	if (!timer_.IsFinish()) {
		transform_.rotate.y += rotationAmount_;
		transform_.rotate.y = Angle::Mod(transform_.rotate.y);
		transform_.translate += move_;

		// 線形補間によってスケールを動かす
		transform_.scale = SoLib::Lerp<Vector3>(startScale_, { 0.0f, 0.0f,0.0f }, SoLib::easeLinear(timer_.GetProgress()));
	}
	else {
		isAlive_ = false;
	}

	// トランスフォームの計算と転送
	transform_.CalcMatrix();
	// タイマー更新
	timer_.Update(deltaTime);
	diplayParticleTimer_.Update(deltaTime);
}
