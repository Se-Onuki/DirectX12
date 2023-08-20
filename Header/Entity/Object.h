#pragma once
#include <unordered_map>
#include <typeindex>
#include <memory>
//#include <type_traits>

#include "../Math/Transform.h"
#include "../../DirectBase/3D/ViewProjection/ViewProjection.h"

class Object;

class IComponent {
	IComponent() = delete;
public:
	IComponent(Object *const object) : object_(object) {}
	virtual ~IComponent() = default;

	virtual void Init() {};
	virtual void Update() {};
	virtual void Draw(const ViewProjection &vp) const {
		vp;
	};

	virtual void ImGuiWidget() {};

	/// @brief 接触時に実行される関数
	/// @param other 接触相手
	virtual void OnCollision(Object *const other) {
		other;
	};

	// 紐づけられた実体
	Object *const object_ = nullptr;
};

class Object {
protected:
	// 生きているか
	bool isActive_;
	// コンポーネントの連想コンテナ
	std::unordered_map<std::type_index, std::unique_ptr<IComponent>> componentMap_;

	// オブジェクトのSRT
	Transform transform_;

	// 今後実装予定
	// Object *parent_ = nullptr;
	// std::list<std::unique_ptr<Object>> children_;

public:
	Object() = default;
	virtual ~Object() = default;

	virtual void Init();
	virtual void Update();
	virtual void Draw(const ViewProjection &vp) const;

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
	const Transform &GetTransform() {
		return transform_;
	}

	const Vector3 &GetWorldPos();

	void OnCollision(Object *const other);

private:

};

template <typename T>
T *const Object::AddComponent() {
	//static_assert(std::is_base_of<IComponent, T>::value, "引数はIComponentクラスの派生クラスではありません");

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