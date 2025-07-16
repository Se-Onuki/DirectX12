#pragma once
#include <iostream>
#include <array>
#include <string>

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
//
//class IsSuccessText {
//public:
//	IsSuccessText() = default;
//	IsSuccessText(const IsSuccessText &success) : isSuccess_(success.isSuccess_) {}
//	IsSuccessText(bool success) : isSuccess_(success ? '\0' : "Failure") {}
//	~IsSuccessText() = default;
//	inline explicit operator bool() const { return isSuccess_.empty(); }
//	inline operator const std::string &() const { return isSuccess_; }
//
//private:
//	std::string isSuccess_ = {};
//};

static inline std::ostream &operator<<(std::ostream &os, const IsSuccess success) {
	constexpr std::array<std::string_view, 2u> str{
		 "Failure", "Success",
	};
	return (os << (static_cast<bool>(success) ? str[1] : str[0]));
}
//
//static inline std::ostream &operator<<(std::ostream &os, const IsSuccessText success) {
//	if (success) {
//		return (os << "Success");
//	}
//	const std::string &text = success;
//	return (os << "Failure : " << text);
//}