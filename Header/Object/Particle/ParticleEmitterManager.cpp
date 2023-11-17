#include "ParticleEmitterManager.h"

void ParticleEmitterManager::Init()
{
	// エミッタリストのクリア
	emitters_.clear();

#ifdef _DEBUG
	

#endif // _DEBUG

}

void ParticleEmitterManager::Update(float deltaTime)
{
	// 終了しているエミッタの削除
	emitters_.remove_if([](std::unique_ptr<IParticleEmitter>& emitter){
		if (emitter->isEnd_) {
			return true;
		}
		return false;
	});

	// 全てのエミッタの更新
	for (std::unique_ptr<IParticleEmitter>& emitter : emitters_)
		emitter->Update(deltaTime);

#ifdef _DEBUG // デバッグ時のみImGuiの描画
	// ImGUiの開始
	ImGui::Begin("ParticleManager");
	// 全パーティクルのImGuiを描画
	ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(0, 100), ImGuiWindowFlags_NoTitleBar);
	// パーティクルが1つでもあった場合
	if (emitters_.size() > 0) {
		for (std::unique_ptr<IParticleEmitter>& emitter : emitters_) {
			emitter->DisplayImGui();
		}
	}
	else // 1つもパーティクルがない場合テキストで表示
		ImGui::Text("No Particles!");
	ImGui::EndChild();
	ImGui::End();
#endif // _DEBUG
}

