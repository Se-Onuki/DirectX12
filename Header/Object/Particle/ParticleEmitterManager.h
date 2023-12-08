#pragma once
#include <map>
#include <string>
#include <typeindex>
#include "ParticleEmitter.h"
#include "ParticleList.h"

/// <summary>
/// パーティクルエミッターの管理マネージャー
/// </summary>
class ParticleEmitterManager
{
private: // コンストラクタ等

	// シングルトンパターンの設定
	ParticleEmitterManager() = default;
	~ParticleEmitterManager() = default;
	ParticleEmitterManager(const ParticleEmitterManager &) = delete;
	const ParticleEmitterManager &operator=(const ParticleEmitterManager &) = delete;

public: // メンバ関数

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static ParticleEmitterManager *GetInstance() {
		static ParticleEmitterManager instance;
		return &instance;
	}

	/// <summary>
	/// 初期化関数
	/// </summary>
	void Init();

	/// <summary>
	/// 更新関数
	/// </summary>
	/// <param name="deltaTime">経過秒数</param>
	void Update(float deltaTime);

	/// <summary>
	/// 引数で指定したパーティクルエミッタを生成する関数
	/// </summary>
	/// <typeparam name="SelectParticle">生成するパーティクルの型</typeparam>
	/// <param name="modelName">粒子モデル名</param>
	/// <param name="aliveTime">パーティクルの継続時間(何も入力しなかった場合はループ)</param>
	/// <returns>そのパーティクルのエミッタ</returns>
	template<IsIParticle SelectParticle>
	inline ParticleEmitter *CreateEmitter(std::string modelName, float aliveTime = 0.0f) {
		// インスタンスの生成
		std::unique_ptr<ParticleEmitter> emitter = std::make_unique<ParticleEmitter>();
		// 生成したインスタンスの初期化
		emitter->Init(modelName, aliveTime);
		// 型タイプを設定
		emitter->SetParticleType<SelectParticle>();
		// 型名取得
		emitter->typeName_ = typeid(SelectParticle).name();
		// エミッタ名の設定

		emitter->name_ = emitter->typeName_;
		int sameNameCount = 0;
		// 全てのエミッタの名前の取得
		for (std::unique_ptr<ParticleEmitter> &emitters : emitters_) {
			// エミッタの名前取得
			std::string GetEmitterName = emitters->name_;

			// オブジェクト名の末尾に数字が含まれている場合は末尾の文字を削除
			while (isdigit(GetEmitterName.at(GetEmitterName.size() - 1))) {
				// 末尾の文字を削除
				GetEmitterName.pop_back();
			}

			if (GetEmitterName == emitter->name_) {
				sameNameCount++;
			}
		}

		// 同名オブジェクトが一個でも存在する場合末尾に番号をつける
		if (sameNameCount > 0) {
			// 数字を文字列に変換
			std::string count = std::to_string((sameNameCount + 1));
			emitter->name_ = emitter->name_ + count;
		}

		// 秒数指定がない場合ループさせる
		if (aliveTime == 0.0f) {
			emitter->isLoop_ = true;
		}

		// エミッタの値を追加
		emitter->AddItem();
		// エミッタの値を読み込む
		emitter->ApplyItem();

		ParticleEmitter *retrurnEmitter = emitter.get();
		// 初期化したインスタンスを配列に追加
		emitters_.push_back(std::move(emitter));

		return retrurnEmitter;
	}

#ifdef _DEBUG
	/// <summary>
	/// パーティクルの型リストに追加する関数
	/// </summary>
	/// <typeparam name="SelectParticleEmitter">追加する型</typeparam>
	template<IsIParticle SelectParticle>
	void AddParticleMold() {
		moldMap_.insert({
			typeid(SelectParticle),
			std::function<std::unique_ptr<IParticle>(const Vector3 &)>([](const Vector3 &position)
				{
					return std::make_unique<SelectParticle>(position);
				})
			});
	}
#endif // _DEBUG

private: // メンバ変数

	// 調整項目クラス
	GlobalVariables *gv_ = nullptr;

	// パーティクルエミッター達
	std::list<std::unique_ptr<ParticleEmitter>> emitters_;

#ifdef _DEBUG // ImGui用変数

	// パーティクル型マップ
	std::map <std::type_index, std::function<std::unique_ptr<IParticle>(const Vector3 &)>> moldMap_;

	// imGui表示用
	std::vector<std::unique_ptr<ParticleEmitter>> imGuiEmitters_;

	// 再生時間
	float imGuiAliveTime_ = 1.0f;
	//ループするか
	bool imGuiIsLoop_ = false;

	// 生成座標
	BaseTransform imGuiEmitTransform_;

#endif // _DEBUG

};