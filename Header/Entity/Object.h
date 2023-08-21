#pragma once
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <type_traits>

#include "../Math/Transform.h"
#include "../../DirectBase/3D/ViewProjection/ViewProjection.h"

class Object;
//class IScene;

///	
/// ↓ コンポーネントの最小構成 ↓
/// 
///	class MyComponent : public IComponent {	← IComponentを継承する
///	public:
///		using IComponent::IComponent;	←	IComponentのコンストラクタを引き継ぐ
///		
///		↓ ここからは自由 ↓
/// 
///		~MyComponent() override {}
/// 
///		[ Init() や Update() などはここに書く ]
///	
///	private:
///	
///	};
///

/// @brief コンポーネント指向の基盤
class IComponent {
	// 通常のコンストラクタは無効化
	IComponent() = delete;
	IComponent(const IComponent &) = delete;
	IComponent operator=(const IComponent &) = delete;
public:
	/// @brief 自動的に呼ばれるコンストラクタ
	/// @param object 紐づけられる実体のアドレス
	IComponent(Object *const object) : object_(object) {}
	virtual ~IComponent() = default;


	/// @brief Objectに追加した時に走る処理
	virtual void Init() {};
	/// @brief 任意で実行する初期化
	virtual void Reset() {};
	/// @brief 毎フレーム実行される処理
	virtual void Update() {};
	/// @brief 描画処理
	/// @param vp ViewProjectionクラス
	virtual void Draw(const ViewProjection &) const {};

	/// @brief ImGuiで表示する内容
	virtual void ImGuiWidget() {};

	/// @brief 接触時に実行される関数
	/// @param Object* other : 接触相手のアドレスが代入される
	virtual void OnCollision(Object *const) {};

	// 紐づけられた実体
	Object *const object_ = nullptr;
};

class Object {
	// 生きているか
	bool isActive_ = false;
	// コンポーネントの連想コンテナ
	std::unordered_map<std::type_index, std::unique_ptr<IComponent>> componentMap_;


	// 今後実装予定
	// 
	// シーン情報
	// IScene * scene_ = nullptr;
	// 
	// 親子関係
	// Object *parent_ = nullptr;
	// std::list<std::unique_ptr<Object>> children_;

public:
	// オブジェクトのSRT
	Transform transform_;

	Object() = default;
	//Object(const Object&) = default;
	virtual ~Object() = default;

	virtual void Init();
	virtual void Reset();
	virtual void Update();
	virtual void Draw(const ViewProjection &vp) const;

	/*template<typename T>
	bool HasComponent();*/

	/// @brief コンポーネントを追加
	/// @tparam T コンポーネントの型
	/// @return コンポーネントのポインタ
	template <typename T>
	T *const AddComponent();

	/// @brief コンポーネントの取得
	/// @tparam T コンポーネント型
	/// @return コンポーネントのポインタ
	template <typename T>
	T *const GetComponent() const;

	void SetActive(const bool newState);
	bool GetActive() const {
		return isActive_;
	}

	const Vector3 &GetWorldPos();

	void OnCollision(Object *const other);

	void ImGuiWidget();

private:

};

template <typename T>
T *const Object::AddComponent() {
	static_assert(std::is_base_of<IComponent, T>::value, "引数はIComponentクラスの派生クラスではありません");

	T *const findComp = GetComponent<T>();
	if (findComp != nullptr) { return findComp; }

	// コンポーネントを生成
	IComponent *const component = new T(this);
	std::type_index key = std::type_index(typeid(T));

	componentMap_[key].reset(component);

	component->Init();

	return GetComponent<T>();
}


template <typename T>
T *const Object::GetComponent() const {
	auto it = componentMap_.find(std::type_index(typeid(T)));
	if (it != componentMap_.end()) {
		return static_cast<T *>(it->second.get());
	}
	return nullptr;
}