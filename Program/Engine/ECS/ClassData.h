#pragma once
#include "../Utils/Containers/Singleton.h"
#include <cstdint>
#include <typeindex>

class ClassData {
public:
	std::type_index typeInfo_ = typeid(void);
	uint32_t size_;
	uint32_t align_;

	void (*constructor_)(void *);

	template<SoLib::IsNotPointer T>
	static ClassData Create() {
		return { .typeInfo_ = typeid(T), .size_ = sizeof(T), .align_ = alignof(T), .constructor_ = [](void *ptr) { new(ptr) T{}; } };
	}

	bool operator==(const ClassData &other) const {
		return (typeInfo_ == other.typeInfo_ && size_ == other.size_);
	}

};


namespace std {
	template<>
	struct hash<ClassData> {
		std::size_t operator()(const ClassData &obj) const {
			// std::type_index のハッシュ値をそのまま返す
			return obj.typeInfo_.hash_code();
		}
	};
}

// クラス情報を管理するマネージャクラス
class ClassDataManager : public SoLib::Singleton<ClassDataManager> {
public:

	struct KeyType {
	private:
		inline static const ClassDataManager *const classManager_ = ClassDataManager::GetInstance();
	public:
		KeyType() = default;
		KeyType(const KeyType &) = default;
		KeyType(const std::type_index &type) :typeIndex_(type) {}
		KeyType(std::type_index &&type) :typeIndex_(type) {}

		std::type_index typeIndex_;
		KeyType &operator=(const std::type_index &index) { return *this = { index }; }
		operator const std::type_index &() const noexcept { return typeIndex_; }

		const ClassData *operator->() const {
			return classManager_->GetClassData(typeIndex_);
		}
		const ClassData *GetClassData() const {
			return classManager_->GetClassData(typeIndex_);
		}

		bool operator==(const KeyType &other) const {
			return this->typeIndex_ == other.typeIndex_;
		}
		bool operator<(const KeyType &other) const {
			return this->typeIndex_ < other.typeIndex_;
		}
	};

private:
	friend SoLib::Singleton<ClassDataManager>;
	std::unordered_map<std::type_index, ClassData> classMap_;

public:
	/// @brief クラス情報を追加する
	/// @tparam T 追加する型
	template<typename T>
	const ClassData *AddClass() {
		// 型情報
		const std::type_index &type = typeid(T);
		// 型情報が保存されているか
		const ClassData *result = GetClassData(type);
		// 保存されていたらそのまま返す
		if (result) { return result; }

		// もし存在しなかったら
		// 型情報を生成する
		classMap_[type] = ClassData::Create<T>();
		// 型情報を返す
		return &classMap_[type];
	}

	/// @brief クラス情報を取得する
	/// @param typeIndex 探す型
	/// @return クラス情報
	const ClassData *GetClassData(const std::type_index &typeIndex) const {
		auto it = classMap_.find(typeIndex);
		if (it != classMap_.end()) {
			return &(it->second);
		}
		return nullptr;
	}
};


namespace std {
	template<>
	struct hash<ClassDataManager::KeyType> {
		std::size_t operator()(const ClassDataManager::KeyType &obj) const {
			// std::type_index のハッシュ値をそのまま返す
			return obj.typeIndex_.hash_code();
		}
	};
}
