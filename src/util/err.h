#pragma once

#include <string>

namespace util {
namespace err {

const std::string strerror();
const std::string strerror(const char *msg);
const std::string strerror(const std::string &msg);
const std::string strerror(const int errnum);
const std::string strerror(const int errnum, const char *msg);
const std::string strerror(const int errnum, const std::string &msg);

} // namespace err
} // namespace util