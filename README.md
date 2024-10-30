# SolEngine

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

- [externals](/Program/externals/)フォルダに､[Imgui](/Program/externals/ImGuiProject/), [Assimp](/Program/externals/assimp/), [nlohmannJson](/Program/externals/nlohmann/), [DirectXTex](/Program/externals/DirectXTex/)を導入しています｡

## ECS (Entity-Component-System)

[ソースコード](/Program/Engine/ECS/)

![ECS Sample](/Images/ECS.png)
Entity[^1]に各種コンポーネントのポインタを持たせるのではなく､Componentを配列として整列させることで､効率的なアクセスが可能になります｡

- [ECSとは (Unity/C#)](https://www.bing.com/ck/a?!&&p=22fc76fd9c67fa0d465343ad4225d48d5da80c2026278fffedf7113d576151f6JmltdHM9MTczMDI0NjQwMA&ptn=3&ver=2&hsh=4&fclid=0aa3bc30-5ae8-629c-3d7e-a89e5b866383&u=a1L3ZpZGVvcy9yaXZlcnZpZXcvcmVsYXRlZHZpZGVvP3E9RUNTK1VuaXR5Jm1pZD01OTg3OTE0NjNDQzU2REUxOEZDMjU5ODc5MTQ2M0NDNTZERTE4RkMyJkZPUk09VklSRQ&ntb=1)
- [参考サイト (C++)](https://qiita.com/harayuu10/items/e15b02e3b0f3081d729b)

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
