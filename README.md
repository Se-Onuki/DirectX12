# SolEngine
[![DebugBuild](https://github.com/Se-Onuki/DirectX12/actions/workflows/DebugBuild.yml/badge.svg?event=push)](https://github.com/Se-Onuki/DirectX12/actions/workflows/DebugBuild.yml) [![DemoBuild](https://github.com/Se-Onuki/DirectX12/actions/workflows/DemoBuild.yml/badge.svg?event=push)](https://github.com/Se-Onuki/DirectX12/actions/workflows/DemoBuild.yml) [![ReleaseBuild](https://github.com/Se-Onuki/DirectX12/actions/workflows/ReleaseBuild.yml/badge.svg?event=push)](https://github.com/Se-Onuki/DirectX12/actions/workflows/ReleaseBuild.yml)

SolEngineはDirectX12を使用したC++ゲームエンジンです｡
アーキタイプ式ECSを主軸に開発しています｡
開発人数は1人です｡

機能
- 
- [アーキタイプECS](/Program/Engine/ECS/)を利用したゲーム設計
- Blenderを使用した[レベルエディタ](/Tools/BlenderAddons/scripts/addons/__pycache__/level_editor.cpython-311.pyc)
- Flyweightパターンに対応した[リソースマネージャ](/Program/Engine/ResourceObject/ResourceObjectManager.h)
- マルチメッシュのOBJ,GLTFの読み込み､描画
- マルチメッシュモデルへのスキニングアニメーション
- ポストエフェクトの簡易適用

## ECS (Entity-Component-System)

[ソースコード](/Program/Engine/ECS/)
`/Program/Engine/ECS/`
- Worldクラスに"CreateEntity"関数を用いてコンポーネントを渡すことで､そのコンポーネントを持ったEntityを生成して､保存できます｡
![ECS UML](/Images/UML.png)

- SystemはSystemExecuterにわたすことで､自動的に型を取得して実行するようになっています｡
![ECS SystemUML](/Images/SystemUML.png)

### 使い方

コンポーネントと呼ばれる構造体を作成し､その構造体に対して実行したいSystemを実行するようにエンジンに指示を送ることで使えます｡

- コンポーネントの作成

[Component.h](/Program/Engine/ECS/Component/Component.hpp)
``` cpp
	// コンポーネント定義
	struct ModelComp : IComponent {
		// 保存するデータの定義
		SolEngine::ResourceHandle<SolEngine::ModelData> model_;

	};
```

---
- 実行するSystemの作成

[System.h](/Program/Engine/ECS/System/NewSystems.h)
```cpp
	// Systemの名前
	class SkinModelDrawer :public IJobEntity {
	public:
		// 読み書き可能で取得するコンポーネントの指定
		ReadAndWrite<ECS::TransformMatComp, ECS::ModelComp, ECS::SkinModel> readWrite_;
		// データの登録
		using DataBase = DataBase<decltype(readWrite_)>;
		// 実行する関数
		void Execute(const World *const, const float);
	};
```
[System.cpp](/Program/Engine/ECS/System/NewSystems.cpp)
```cpp
	// Systemの関数定義
	void SkinModelDrawer::Execute(const World *const, const float)
	{
		// レンダラの取得
		SkinModelHandleListManager *const skinModelRender_ = SkinModelHandleListManager::GetInstance();
		// 取得したコンポーネントの展開
		auto &[transform, model, skinModel] = readWrite_;
		// レンダラにデータを送る
		skinModelRender_->AddBox({ model.model_, skinModel.skinModel_ }, { .transMat_ = transform });
	}
```

**実際の流れ**
```cpp

	ECS::NewWorld world;		// ワールドを生成
	ECS::Prefab prefab;		// プレハブの生成
	prefab += ECS::IsAlive{};	// プレハブに生存フラグのコンポーネントを追加
	prefab += ECS::ModelComp{};	// モデルデータのコンポーネントを追加

	world.CreateEntity(prefab);	// プレハブをワールドに追加

	ECS::SystemExecuter systemExecuter;	// システムの実行クラスを作成

	systemExecuter.AddSystem<ECS::System::Par::SkinModelDrawer>();	// スキニングモデルの描画Systemの追加

	// ゲームループ
	while(true){
	systemExecuter_.Execute(&world, deltaTime);	// システムの実行
	}

```

## 外部ライブラリ


- [Program/externals](/Program/externals/)
	- [Imgui](https://github.com/ocornut/imgui)
	- [Assimp](https://github.com/assimp/assimp)
	- [nlohmannJson](https://github.com/nlohmann/json)
	- [DirectXTex](https://github.com/microsoft/DirectXTex)


## 作成中のゲーム
### ECS Survivor
![GameScreenShot](/Images/GameScreenShot.png)
- ECS Survivorという､[SoulStoneSurvivor](https://store.steampowered.com/app/2066020/Soulstone_Survivors/)を参考にしたゲームを作成しています｡
![目標1](https://shared.fastly.steamstatic.com/store_item_assets/steam/apps/2066020/ss_6d985af64f465cf2538e045c1ce5315f61756f99.116x65.jpg?t=1730207613) ![目標2](https://shared.fastly.steamstatic.com/store_item_assets/steam/apps/2066020/ss_df8d10b4b673d03554087bb02b8b849a0781d38c.116x65.jpg?t=1730207613)

- 大量のオブジェクトを処理できるエンジンを目指しているため､原作のワチャワチャ感を目指して行きたいです｡
