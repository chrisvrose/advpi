#include "util/runner.hh"

#include <format>
#include <iostream>

/**
 * Runs tests.
 * Returns number of failed tests
 */
int runTests(std::vector<std::function<void()>> tests, bool verbose) {
    int numFailed = 0, numTest = 1;
    for (auto test : tests) {
        try {
            test();
            if (verbose) {
                std::cout << "Test " << numTest << " passed!" << std::endl;
            }
        } catch (std::string error) {
            std::cout << "Test " << numTest << " failed!" << std::endl;
            std::cout << "Error: " << error << std::endl;
            numFailed++;
        }
        numTest++;
    }
    if (verbose) {
        std::cout << std::format("Failed {} tests", numFailed) << std::endl;
    }
    return numFailed;
}
