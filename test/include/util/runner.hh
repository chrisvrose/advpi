#pragma once
#include <functional>
#include <vector>
int runTests(std::vector<std::function<bool()>> tests, bool verbose=false);
