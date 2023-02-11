#include <algorithm>
#include <ios>
#include <iomanip>
#include "string.h"

namespace util {
namespace string {

static const std::regex rex_left_spaces {R"(^\s*)"};
static const std::regex rex_right_spaces {R"(\s*$)"};
static const std::regex rex_spaces {R"(^\s*(.*\S)\s*$)"};

std::string &trim_left(std::string &s)
{
    std::string trimmed {std::regex_replace(s, rex_left_spaces, "")};
    s.swap(trimmed);
    return s;
}

std::string &trim_right(std::string &s)
{
    std::string trimmed {std::regex_replace(s, rex_right_spaces, "")};
    s.swap(trimmed);
    return s;
}

std::string &trim(std::string &s)
{
    std::string trimmed {std::regex_replace(s, rex_spaces, "$1")};
    s.swap(trimmed);
    return s;
}

bool starts_with(const std::string &s, const std::string &str)
{
    return std::equal(s.cbegin(), s.cbegin() + static_cast<std::string::difference_type>(str.size()), str.begin());
}

bool ends_with(const std::string &s, const std::string &str)
{
    return std::equal(s.cend() - static_cast<std::string::difference_type>(str.size()), s.cend(), str.begin());
}

/**
 *  print each byte value in buffer  as hex string
 */
std::string make_hex_str(const uint8_t* buf, const uint32_t size, bool use_uppercase, bool insert_spaces)
{
    // setw() : 최소 width 지정
    // setfill() : (빈위치에 삽입될)채움문자 지정.

    std::ostringstream ss;
    ss << std::hex << std::setfill('0');
    if (use_uppercase) ss << std::uppercase;

    for (size_t i = 0; i < size; ++i)
    {
        ss << std::setw(2) << static_cast<int>(buf[i]);//(*buf++);
        if (insert_spaces) ss << " ";
    }
    return ss.str();
}

} // namespace string
} // namespace util