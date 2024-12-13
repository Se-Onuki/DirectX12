/// @file ThreadPool.h
/// @brief スレッドプール(未実装)
/// @author ONUKI seiya
#pragma once
#include "../../Engine/Utils/Containers/Singleton.h"

class ThreadPoolManager : public SoLib::Singleton<ThreadPoolManager> {

	friend SoLib::Singleton<ThreadPoolManager>;
	ThreadPoolManager() = default;
	~ThreadPoolManager() = default;

public:



};
