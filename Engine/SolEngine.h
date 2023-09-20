#pragma once

class SolEngine {
	SolEngine() = default;
	SolEngine(const SolEngine &) = delete;
	SolEngine operator=(const SolEngine &) = delete;
	SolEngine operator=(SolEngine &&) = delete;
	~SolEngine() = default;
public:

	static SolEngine *const GetInstance() {
		static SolEngine instance{};
		return &instance;
	}

	//static Init()

private:

};