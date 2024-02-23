#pragma once

namespace ECS {

    enum class SimulationSystemGroup {
        BeginSimulationEntityCommandBufferSystem, 	// 
        TransformSystemGroup,						// Transformを変更する処理
        EndFrameParentSystem,						// 親のTransformからグローバル座標を計算する
        CopyTransformFromGameObjectSystem,			// GameObjectからTransformをコピーする
        EndFrameTRSToLocalToWorldSystem,			// ローカル座標からワールド座標に変換する
        EndFrameTRSToLocalToParentSystem,			// 
        EndFrameLocalToParentSystem,				// 
        CopyTransformToGameObjectSystem,			// TransformをGameObjectにコピーする
        LateSimulationSystemGroup,					// シミュレーションの後処理
        EndSimulationEntityCommandBufferSystem,		// エンティティコマンドバッファの終了処理

        kMaxSize
    };

    class ISystem {
    public:
        SimulationSystemGroup systemGroup_;
        virtual ~ISystem() = 0;
        virtual void OnUpdate(const float deltaTime) = 0;

        /// @brief Worldに変更をもたらさないか
        /// @return 変更をもたらさないならtrue
        virtual bool IsConstSystem() const = 0;

        /// @brief シミュレーションシステムのグループを設定
        void SetSimlationSystemGroup(SimulationSystemGroup group) { systemGroup_ = group; }

        /// @brief シミュレーションシステムのグループを取得
        /// @return シミュレーションシステムのグループ
        SimulationSystemGroup GetSimlationSystemGroup() const { return systemGroup_; }
    };

    class IJobSystem {
    public:
        virtual ~IJobSystem() = 0;

        virtual void OnUpdate(const float deltaTime) = 0;
    };

}