/// @file LeakChecker.cpp
/// @brief すべてのデータが開放された時に開放を行う
/// @author ONUKI seiya
#include "LeakChecker.h"

const DirectResourceLeakChecker leakChecker{};
uint32_t DirectResourceLeakChecker::loadCount = 0u;