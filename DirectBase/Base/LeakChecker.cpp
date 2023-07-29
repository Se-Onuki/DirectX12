#include "LeakChecker.h"

const DirectResourceLeakChecker leakChecker{};
uint32_t DirectResourceLeakChecker::loadCount = 0u;