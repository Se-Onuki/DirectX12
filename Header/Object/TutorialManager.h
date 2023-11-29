#pragma once
#include <cstdint>
class TutorialManager
{
	TutorialManager() = default;
	TutorialManager(const TutorialManager &) = delete;
	TutorialManager &operator=(const TutorialManager &) = delete;
	~TutorialManager() = default;
public:

	enum class TutorialProgress : int32_t {
		kWalk,
		kFloatZ,
		kRotate,
		kDownZ,
		kToGoal,
	};

	static auto *const GetInstance() {
		static TutorialManager instance{};
		return &instance;
	}

	void Init() {
		progress_ = 0;
	}

	int32_t GetProgress() const { return progress_; }

	void SetProgress(const TutorialProgress value) {
		progress_ = static_cast<int32_t>(value);
	}
	void AddProgress() { progress_++; }

private:

	int32_t progress_;

};