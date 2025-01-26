#pragma once
#include <iostream>
#include <array>

/// @brief 成否判定
/// @details boo成功ならtrue、失敗ならfalse
class IsSuccess {
public:

	IsSuccess() = default;
	IsSuccess(bool success) : isSuccess_(success) {};
	~IsSuccess() = default;

	inline explicit operator bool() const { return isSuccess_; }

private:
	bool isSuccess_ = false;
};

static inline std::ostream &operator<<(std::ostream &os, const IsSuccess succes) {
	constexpr std::array<std::string_view, 2u> str{
		 "Failure", "Success",
	};
	return (os << (static_cast<bool>(succes) ? str[1] : str[0]));
}