#pragma once
#include "../../../Utils/Containers/Singleton.h"

class ThreadPoolManager : public SoLib::Singleton<ThreadPoolManager> {

	friend SoLib::Singleton<ThreadPoolManager>;
	ThreadPoolManager() = default;
	~ThreadPoolManager() = default;

public:



};
