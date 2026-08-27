#include "util/runner.hh"
#include <iostream>

/**
 * Runs tests.
 * Returns number of failed tests
 */
int runTests(std::vector<std::function<bool()>> tests, bool verbose){
    int numFailed = 0,numTest = 1;
    for (auto test : tests) {
        auto retVal = test();
        if (retVal) {
            if(verbose)
                std::cout << "Test " << numTest << " passed!" << std::endl;
        }else{
            std::cout << "Test " << numTest << " failed!" << std::endl;
            numFailed++;
        }
        numTest++;
    }
    return numFailed;
}
