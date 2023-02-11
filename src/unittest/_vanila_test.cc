#include <chrono>
#include <iostream>
#include <sstream>

#include "gtest/gtest.h"


namespace {

/***** DO NOT REMOVE *****/
#define STARTTIME ElapsedTime et;
#define ENDTIME et.elapsed();
using namespace std::chrono;
class ElapsedTime {
public:
    ElapsedTime() { steadyclk_start = steady_clock::now(); }
    void elapsed() {
        steadyclk_end = steady_clock::now();
        std::cout << "[**********] Elapsed time: "
            << duration_cast<microseconds>(steadyclk_end - steadyclk_start).count()
            << " us.\n";
    }

private:
    steady_clock::time_point steadyclk_start;
    steady_clock::time_point steadyclk_end;
};


/***** GLOBAL UNIT CONTEXT UNDER HERE *****/

class VanilaTest: public ::testing::Test {
protected:
    // You can remove any or all of the following functions if its body
    // is empty.
    VanilaTest() {
        // You can do set-up work for each test here.
        // No exception handling allowed here.
    }

    virtual ~VanilaTest() {
        // You can do clean-up work that doesn't throw exceptions here.
    }

    virtual void SetUp() override {
        // Code here will be called immediately after each test
        // (right before each test).
    }

    virtual void TearDown() override {
        // Code here will be called immediately after each test
        // (right before the destructor).
        // No exception handling allowed here.
    }
};


/***** TEST UNIT CONTEXT UNDER HERE *****/

TEST_F(VanilaTest, test1)
{STARTTIME

    for (auto id = 0; id < 10; ++id) {
        std::cout << "[" << id << "]" << std::endl;
    }

ENDTIME}

} ///namespace
