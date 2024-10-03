#pragma once
#include <typeindex>
#include <typeinfo>
#include <unordered_set>
#include <vector>
#include "../../Engine/Utils/SoLib/SoLib.h"
#include "../Archetype.h"

#include <assert.h>

//#include "Header/Component/Component.hpp"
#include <any>

#include <bit>

#include <stdlib.h>
#include <unordered_map>
namespace ECS {
	class Entity;
}

class CustomArray {
public:
	CustomArray() : maxSize_(1), data_(nullptr), classData_({}), nowSize_(0) {}

	~CustomArray() { Delete(); }

	void Delete() {
		if (data_) {
			free(data_);
		}
	}

	void Init(const ClassData &classData, uint32_t size = 1) {
		Delete();
		maxSize_ = size;
		classData_ = classData;
		nowSize_ = 0;
		data_ = malloc(size * classData_.size_);
	}

	bool Resize(uint32_t newSize) {
		if (newSize <= maxSize_) // サイズが小さかった場合は無効
			return true;

		void *newData = realloc(data_, newSize * classData_.size_);
		if (!newData) // 取得に失敗した場合は無効
			return false;

		data_ = newData;
		maxSize_ = newSize;
		return true; // 拡大成功
	}

	template<typename T> bool push_back() {
		if (nowSize_ == maxSize_) {
			if (!Resize(maxSize_ * 2))
				return false;
		}
		nowSize_++;
		void *ptr = (*this)[nowSize_ - 1];
		new (ptr) T();
		return true;
	}

	template<typename T> bool push_back(const T &value) {
		if (nowSize_ == maxSize_) {
			if (!Resize(maxSize_ * 2))
				return false;
		}
		nowSize_++;
		void *ptr = (*this)[nowSize_ - 1];
		set(nowSize_ - 1, value);
		return true;
	}

	/// @brief 要素の入れ替え
	/// @param index1 要素A
	/// @param index2 要素B
	/// @return 入れ替えの成否
	bool swap(uint32_t index1, uint32_t index2) {
		if (empty() || index1 >= nowSize_ ||
			index2 >= nowSize_) { // 保存量が0 or 添え字がオーバーしている時は無効
			return false;
		}
		void *ptr1 = (*this)[index1];
		void *ptr2 = (*this)[index2];

		void *swapBuff = malloc(classData_.size_); // 一時保存用のバッファ
		memcpy(swapBuff, ptr1, classData_.size_);  // 入れ替え
		memcpy(ptr1, ptr2, classData_.size_);
		memcpy(ptr2, swapBuff, classData_.size_);
		free(swapBuff);
		return true;
	}

	/// @brief 要素の削除
	/// @param index 削除する添え字
	/// @return 削除の成否
	bool erase(uint32_t index) {
		if (empty() || index >= nowSize_) { // 保存量が0 or 添え字がオーバーしている時は無効
			return false;
		}
		void *ptr = (*this)[index];
		memmove(
			ptr, static_cast<char *>(ptr) + classData_.size_,
			(nowSize_ - index - 1) * classData_.size_);
		nowSize_--;
		return true;
	}

	/// @brief 要素の特殊削除
	/// @param index 削除する添え字
	/// @return 削除の成否
	bool swapErase(uint32_t index) {
		if (empty() || index >= nowSize_) { // 保存量が0 or 添え字がオーバーしている時は無効
			return false;
		}

		void *ptr = (*this)[index];

		void *lastPtr = end();
		memcpy(ptr, lastPtr, classData_.size_);

		nowSize_--;
		return true;
	}

	template<typename T> const T &get(const uint32_t &index) {
		if (empty() || index >= nowSize_) {
			return nullptr;
		}
		return GetArray<T>()[index];
	}

	template<typename T> bool set(const uint32_t &index, const T &value) {
		if (empty() || index >= nowSize_) {
			return false;
		}
		GetArray<T>()[index] = value;
		// new (&(GetArray<T>()[index])) T(value);
		return true;
	}

	template<typename T> bool setOrPush(const uint32_t &index, const T &value) {
		if (set(index, value))
			return true;
		else
			return push_back(value);
	}

	bool passOtherArray(CustomArray &other, uint32_t index) {
		void *ptr = (*this)[index];
		if (!ptr)
			return false;
		other.push_back(ptr, classData_.size_);

		swapErase(index);
		return true;
	}

	/// @brief 要素へのアクセス
	/// @param index 添え字
	/// @return 要素のアドレス
	void *operator[](const uint32_t &index) {
		if (empty() || index >= nowSize_) {
			return nullptr;
		}
		return static_cast<char *>(data_) + classData_.size_ * index;
	}

	/// @brief 始点の要素のアドレス
	/// @return 始点アドレス
	void *begin() const { return data_; }

	/// @brief 末端の要素のアドレス
	/// @return 末端アドレス
	void *end() const {
		if (empty())
			return data_;
		return static_cast<char *>(data_) + classData_.size_ * (nowSize_ - 1);
	}

	uint32_t size() const { return nowSize_; }

	bool empty() const { return nowSize_ == 0; }
	/// @brief 配列へのアクセス
	/// @tparam T 型名
	/// @return 型変換がされたポインタ
	template<SoLib::IsNotPointer T>
	T *GetArray() { return static_cast<T *>(data_); }

private:
	void push_back(const void *value, size_t size) {
		if (nowSize_ == maxSize_) {
			Resize(maxSize_ * 2);
		}
		void *ptr = static_cast<char *>(data_) + nowSize_ * classData_.size_;
		memcpy(ptr, value, size);
		nowSize_++;
	}

	uint32_t maxSize_;
	void *data_;
	ClassData classData_;
	uint32_t nowSize_;
};

class Chunk {
public:
	Chunk(const Archetype &archetype, const uint32_t &maxSize = 16) { CreateArray(archetype, maxSize); }
	Chunk(const Chunk &otherChunk, const uint32_t &maxSize) : archetype_(otherChunk.archetype_), maxCount_(maxSize) {
		//std::unordered_map<std::type_index, CustomArray> componentArray_;
	}

	Chunk() {}
	~Chunk() {}

	void CopyChunk(Chunk &other) {
		other.entityCount_ = entityCount_;
		other.archetype_ = archetype_;
		// for ()
	}

	void CreateArray([[maybe_unused]]const Archetype &archetype, [[maybe_unused]] const uint32_t &maxSize = 16) {

		/*const auto *compRegistry = ECS::ComponentRegistry::GetInstance();

		for (uint32_t i = 0; i < archetype.compFlag_.Get().size(); i++) {
			if (archetype.compFlag_.Get().test(i)) {
				const auto &comp = compRegistry->typeDatas_[i];
				componentList_[comp.typeIndex_].Init()
			}
		}

		for (const auto &element : ) {
			componentList_[element.typeIndex_].Init(*element.GetClassData(), maxSize);
		}
		archetype_ = archetype;*/
	}

	template<typename T> T *GetArray() {
		return (componentList_[typeid(T)].GetArray<T>());
	}

	template<typename T> CustomArray &GetCustomArray() {
		return componentList_[typeid(T)];
	}

	void ChankDelete() {}

	const Archetype &GetArchetype() { return archetype_; }

	const uint32_t &size() { return entityCount_; }
	uint32_t entityCount_ = 0;
	const uint32_t maxCount_ = 1;

private:
	Archetype archetype_;
	std::unordered_map<std::type_index, CustomArray> componentList_ = {};
};

// class CustomArray {
// public:
//	CustomArray(const ClassData &classData) {
//
//	}
//
//
// private:
//	void* array_;
//	uint32_t classSizeof_;
// };
