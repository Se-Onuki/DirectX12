#include "ParticleEmitterManager.h"

void ParticleEmitterManager::Init()
{
	// エミッタリストのクリア
	emitters_.clear();

#ifdef _DEBUG
	
	// デバッグ用の型を追加
	AddParticleMold<StarParticle>(); // 星パーティクル

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
	// パーティクルが1つでもあった場合
	if (emitters_.size() > 0) {
		for (std::unique_ptr<ParticleEmitter> &emitter : emitters_) {
			ImGui::Text(emitter->name_.c_str());
		}
	}
	else { // 1つもパーティクルがない場合テキストで表示
		ImGui::Text("No Particles!");
	}
	ImGui::EndChild();

	// イテレータの取得
	auto iter = moldMap_.begin();
	// パーティクルのモデルと集合を1つづつ取得
	while (iter != moldMap_.end()) {
		if (ImGui::Button(iter->first.name())) {
			// インスタンスの生成
			std::unique_ptr<ParticleEmitter> newEmitter = std::make_unique<ParticleEmitter>();
			// 生成したインスタンスの初期化
			newEmitter->Init(imGuiAliveTime_);
			// 型タイプを設定
			newEmitter->type_ = iter->second;
			// 名前を設定
			newEmitter->name_ = iter->first.name();
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

			// 初期化したインスタンスを配列に追加
			emitters_.push_back(std::move(newEmitter));
		}
		iter++;
	}

	// 再生時間の設定
	ImGui::DragFloat("PlayTime", &imGuiAliveTime_, 0.01f, 15.0f);

	ImGui::End();
#endif // _DEBUG
}

