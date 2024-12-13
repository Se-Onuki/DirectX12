/// @file ISystem.h
/// @brief システムの基底
/// @author ONUKI seiya
#pragma once

class World;
namespace ECS {

	enum class SimulationSystemGroup {
		BeginSimulationEntityCommandBufferSystem, 	// 外部からデータを受け取る処理
		TransformSystemGroup,						// Transformを変更する処理
		EndFrameParentSystem,						// 親のTransformからグローバル座標を計算する
		CopyTransformFromGameObjectSystem,			// GameObjectからTransformをコピーする
		EndFrameTRSToLocalToWorldSystem,			// SRT->ローカル座標->ワールド座標に変換する
		EndFrameTRSToLocalToParentSystem,			// SRT->ローカル座標->子のローカル座標に渡す
		EndFrameLocalToParentSystem,				// 親のローカル座標を使ってローカル座標を計算する
		CopyTransformToGameObjectSystem,			// TransformをGameObjectにコピーする
		LateSimulationSystemGroup,					// シミュレーションの後処理
		EndSimulationEntityCommandBufferSystem,		// 外部にデータを送る処理

		kMaxSize
	};

	class ISystem {
	public:
		//SimulationSystemGroup systemGroup_;
		virtual ~ISystem() = default;
		virtual void OnUpdate(::World *world, const float deltaTime) = 0;

		/// @brief Worldに変更をもたらさないか
		/// @return 変更をもたらさないならtrue
		virtual bool IsConstSystem() const = 0;

		/// @brief シミュレーションシステムのグループを設定
		//void SetSimlationSystemGroup(SimulationSystemGroup group) { systemGroup_ = group; }

		/// @brief シミュレーションシステムのグループを取得
		/// @return シミュレーションシステムのグループ
		//SimulationSystemGroup GetSimlationSystemGroup() const { return systemGroup_; }
	};

	//class IJobEntity {
	//public:
	//	virtual ~IJobEntity() = default;

	//	virtual void OnUpdate(const float deltaTime) = 0;
	//};


	//class IJobEntity {
	//public:

	//	virtual void Execute(const float deltaTime) = 0;
	//};

}