#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

namespace util {
namespace pipe {

/* HOW TO USE
int main () {
    std::cout << util::pipe::fpexec(R"(readlink /proc/1/exe)") << std::endl;
    return 0;
}
*/
/* Pre-C++11 */
std::string fexec(const char* cmd) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}


/* HOW TO USE
int main () {
    std::cout << util::pipe::pexec(R"(readlink /proc/1/exe)") << std::endl;
    return 0;
}
*/
/* Post-C++11 */
std::string pexec(const char* cmd) {
    std::array<char, 256> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}



} //namespace pipe
} //namespace util