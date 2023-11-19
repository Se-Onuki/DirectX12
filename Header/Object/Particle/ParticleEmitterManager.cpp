#include "ParticleEmitterManager.h"

void ParticleEmitterManager::Init()
{
	// インスタンスを取得
	gv_ = GlobalVariables::GetInstance();

	// エミッタリストのクリア
	emitters_.clear();

	// 各種パーティクルの固有パラメーターを調整項目クラスから読み込み
	StarParticle::AddItem();
	StarParticle::ApplyItem();

#ifdef _DEBUG
	
	// デバッグ用の型を追加
	AddParticleMold<TestParticle>(); // テストパーティクル
	AddParticleMold<StarParticle>(); // 星パーティクル

	// パーティクルの全数値の管理
	// イテレータの取得
	auto iter = moldMap_.begin();
	// イテレータの終わりまでループ
	while (iter != moldMap_.end()) {
		// インスタンスの生成
		ParticleEmitter* newEmitter = new ParticleEmitter();
		// 生成したインスタンスの初期化
		newEmitter->Init(imGuiAliveTime_);
		// 型タイプを設定
		newEmitter->type_ = iter->second;
		// エミッタの座標設定
		newEmitter->emitTransform_ = imGuiEmitTransform_;
		// 名前を設定
		newEmitter->typeName_ = iter->first.name(); // 生成するパーティクル型名の取得

		newEmitter->AddItem();
		newEmitter->ApplyItem();
		imGuiEmitters_.push_back(newEmitter);
		iter++;
	}

#endif // _DEBUG

}

void ParticleEmitterManager::Update(float deltaTime)
{
	// 終了しているエミッタの削除
	emitters_.remove_if([](std::unique_ptr<ParticleEmitter> &emitter)
		{
			if (emitter->isEnd_) {
				return true;
			}
			return false;
	});

	// 全てのエミッタの更新
	for (std::unique_ptr<ParticleEmitter> &emitter : emitters_) {
		emitter->Update(deltaTime);
	}

#ifdef _DEBUG // デバッグ時のみImGuiの描画
	// ImGUiの開始
	ImGui::Begin("ParticleManager");
	// 全パーティクルのImGuiを描画
	ImGui::BeginChild(ImGui::GetID((void *)0), ImVec2(0, 100), ImGuiWindowFlags_NoTitleBar);
	// カウント用
	int emitterCount = 0;
	// パーティクルが1つでもあった場合
	if (emitters_.size() > 0) {
		for (std::unique_ptr<ParticleEmitter> &emitter : emitters_) {
			ImGui::Text(emitter->name_.c_str());
			ImGui::SameLine();
			if(!emitter->emitAliveTimer_.IsFinish())
				ImGui::Text(" : Playing");
			else
				ImGui::Text(" : Ending");
			ImGui::SameLine();
			std::string name = "End : " + std::to_string((emitterCount));
			if (ImGui::Button(name.c_str()))
				emitter->Finish();

			emitterCount++;
		}
	}
	else { // 1つもパーティクルがない場合テキストで表示
		ImGui::Text("No Particles!");
	}
	ImGui::EndChild();

	// イテレータの取得
	auto iter = moldMap_.begin();
	// イテレータの終わりまでループ
	while (iter != moldMap_.end()) {
		if (ImGui::Button(iter->first.name())) {
			// インスタンスの生成
			std::unique_ptr<ParticleEmitter> newEmitter = std::make_unique<ParticleEmitter>();
			// 生成したインスタンスの初期化
			newEmitter->Init(imGuiAliveTime_);
			// 型タイプを設定
			newEmitter->type_ = iter->second;
			// エミッタの座標設定
			newEmitter->emitTransform_ = imGuiEmitTransform_;
			// 名前を設定
			newEmitter->typeName_ = iter->first.name(); // 生成するパーティクル型名の取得
			newEmitter->name_ = iter->first.name();		// エミッタ自体の名前
			int sameNameCount = 0;
			// 全てのエミッタの名前の取得
			for (std::unique_ptr<ParticleEmitter>& emitter : emitters_) {
				// エミッタの名前取得
				std::string GetEmitterName = emitter->name_;
				
				// オブジェクト名の末尾に数字が含まれている場合は末尾の文字を削除
				while (isdigit(GetEmitterName.at(GetEmitterName.size() - 1)))
				{
					// 末尾の文字を削除
					GetEmitterName.pop_back();
				}

				if (GetEmitterName == newEmitter->name_) {
					sameNameCount++;
				}
			}

			// 同名オブジェクトが一個でも存在する場合末尾に番号をつける
			if (sameNameCount > 0) {
				// 数字を文字列に変換
				std::string count = std::to_string((sameNameCount + 1));
				newEmitter->name_ = newEmitter->name_ + count;
			}

			// エミッタの値を追加
			newEmitter->AddItem();
			// エミッタの値を読み込む
			newEmitter->ApplyItem();

			// 初期化したインスタンスを配列に追加
			emitters_.push_back(std::move(newEmitter));
		}

		
		iter++;
	}
	// 生成座標の設定
	imGuiEmitTransform_.ImGuiWidget();
	// 再生時間の設定
	ImGui::DragFloat("PlayTime", &imGuiAliveTime_, 0.01f, 15.0f);
	ImGui::End();

	// パーティクルの全数値の管理
	// イテレータの取得
	int count = 0;
	auto iIter = moldMap_.begin();
	// イテレータの終わりまでループ
	while (iIter != moldMap_.end()) {
		ImGui::Begin("ParticleParameters", nullptr, ImGuiWindowFlags_MenuBar);
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu(iIter->first.name())) {
				imGuiEmitters_[count]->DisplayImGui();
				ImGui::EndMenu();
			}
		}
		ImGui::EndMenuBar();
		iIter++;
		count++;
		ImGui::End();
	}
	
	StarParticle::DisplayImGui();

#endif // _DEBUG
}

