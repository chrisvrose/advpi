#pragma once
#include <format>
#include <functional>
#include <vector>

int runTests(std::vector<std::function<void()>> tests, bool verbose=false);

template <typename T1,typename T2>
void assert_eq(T1 expected, T2 actual) {
    bool eq = expected == actual;
    if (!eq) {
        throw std::format("Expected {}, got {}",expected, actual);
    }
}

inline void assert_true(bool val){
    if (!val) {
        throw std::format("Expected true, got false");
    }
}
