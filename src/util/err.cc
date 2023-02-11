#include <array>
#include <cstring>
#include "err.h"

namespace util {
namespace err {

const std::string strerror()
{
    return strerror(errno);
}

const std::string strerror(const std::string &msg)
{
    return msg + ": " + strerror();
}

const std::string strerror(const char *msg)
{
    return strerror(std::string{msg});
}

const std::string strerror(const int errnum)
{
    std::array<char, 1024> errmsg{0, };
    char *buf {errmsg.begin()};

#if (_POSIX_C_SOURCE >= 200112L) && !_GNU_SOURCE
    strerror_r(errnum, buf, errmsg.size());
#else
    buf = strerror_r(errnum, buf, errmsg.size());
#endif

    return buf;
}

const std::string strerror(const int errnum, const std::string &msg)
{
    return msg + ": " + strerror(errnum);
}

const std::string strerror(const int errnum, const char *msg)
{
    return strerror(errnum, std::string{msg});
}

} // namespace err
} // namespace util
