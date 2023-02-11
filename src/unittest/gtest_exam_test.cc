#include <bits/stdc++.h>

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

class TempTest: public ::testing::Test {
protected:
    // You can remove any or all of the following functions if its body
    // is empty.
    TempTest() {
        // You can do set-up work for each test here.
        // No exception handling allowed here.
    }

    virtual ~TempTest() {
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

using namespace std;

struct PACKTEST{
    int a;
    char b;
    double c;
};

TEST_F(TempTest, rund)
{STARTTIME

    cout << (std::isspace('L', std::cin.getloc()) ? "true" : "false") << endl; // false
    cout << (std::isspace('\n', std::cin.getloc()) ? "true" : "false") << endl; // true
    cout << (std::isspace('\t', std::cin.getloc()) ? "true" : "false") << endl;  // true
    cout << (std::isspace(' ', std::cin.getloc()) ? "true" : "false") << endl;   // true


    /**
        https://stackoverflow.com/questions/25552822/why-does-pointer-to-int-convert-to-void-but-pointer-to-function-convert-to-bool
        N3337 - 4.12 Boolean conversions
        A zero value, null pointer value, or null member pointer value is converted to false;
        any other value is converted to true
    */
    int i = 10;
    void* nnp = &i;
    void* np = nullptr;
    void* nl = NULL;

    auto chk = [](void* p) {if (p) {return "true";} else { return "false";}};
    cout << chk(nnp) << endl;   // true
    cout << chk(np) << endl;    // false
    cout << chk(nl) << endl;    // false


ENDTIME}

} /// namespace
