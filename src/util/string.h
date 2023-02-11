//string.h
#pragma once

#include <algorithm>
#include <string>
#include <regex>
#include <vector>
#include <type_traits>
#include <iomanip>
#include <sstream>
#include <cstddef>
#include <cctype>
#include "net.h"

namespace util {
namespace string {

template <class T>
inline std::string tostring(const T& v)
{
    std::ostringstream os;
    os << v;
    return os.str();
}

inline std::string &toupper(std::string &s) {
    // http://en.cppreference.com/w/cpp/string/byte/toupper
    // input should be converted into "unsigned char" first.
    const auto _toupper {[] (unsigned char c){return std::toupper(c);}};
    std::transform(s.begin(), s.end(), s.begin(), _toupper);
    return s;
}

inline std::string toupper(std::string &&s) {
    std::string str {s};
    return toupper(str);
}

inline std::string &tolower(std::string &s) {
    const auto _tolower {[] (unsigned char c){return std::tolower(c);}};
    std::transform(s.begin(), s.end(), s.begin(), _tolower);
    return s;
}

inline std::string &tolower(std::string &&s) {
    std::string str {s};
    return tolower(str);
}

std::string &trim_left(std::string &s);
std::string &trim_right(std::string &s);
std::string &trim(std::string &s);

inline std::string &trim_left(std::string &s, const std::string &drop) {
    s.erase(0, s.find_first_not_of(drop));
    return s;
}

inline std::string &trim_right(std::string &s, const std::string &drop) {
    s.erase(s.find_last_not_of(drop) + 1);
    return s;
}

inline std::string &trim(std::string &s, const std::string &drop) {
    return trim_right(trim_left(s, drop), drop);
}

template<class T = std::vector<std::string>>
typename std::enable_if<std::is_same<std::string, typename T::value_type>::value, T>::type
split(const typename T::value_type &s)
{
    T container;
    if (s.empty()) return container;

    if (std::isspace(s[0])) container.emplace_back();

    std::istringstream is{s};
    while (is) {
        if (std::isspace(is.peek())) {
            is.get();
            while (std::isspace(is.peek())) {
                is.get();
                container.emplace_back();
            }
        } else {
            typename T::value_type ss;
            is >> ss;
            container.emplace_back(ss);
        }
    }
    container.pop_back();

    return container;
}

template<class T = std::vector<std::string>>
typename std::enable_if<std::is_same<std::string, typename T::value_type>::value, T>::type
split(const typename T::value_type &s, const typename T::value_type &delims)
{
    T container;
    if (s.empty()) return container;

    if (delims.find(s[0]) != delims.npos) container.emplace_back();

    size_t begin, end {0};
    const auto proceed {[&] {
        begin = end;
        end = s.find_first_of(delims, begin);
        return end != s.npos;
    }};

    while (proceed()) {
        if (begin == end) {
            while (delims.find(s[++end]) != delims.npos) container.emplace_back();
        } else {
            container.emplace_back(s.substr(begin, end - begin));
        }
    }

    if (begin != s.npos) {
        container.emplace_back(s.substr(begin));
    } else if (s.find_last_of(delims, s.length() - 1) == s.length() - 1) {
        container.emplace_back();
    }

    return container;
}

template<class ForwardIt>
std::string to_string(
        ForwardIt first,
        ForwardIt last,
        char delim = ' ',
        decltype(std::hex) base = std::hex)
{
    std::ostringstream os;

    base(os);
    os.fill('0');

    const int w { base == std::hex ? 2 : (base == std::dec || base == std::oct) ? 3 : 0};
    while (first != last) {
        if (delim && os.tellp()) os << delim;
        os << std::setw(w) << static_cast<int>(static_cast<unsigned char>(*first++));
    }

    return os.str();
}

template<class Iterable>
std::string join(const Iterable& cont, const std::string& sep) {
    std::ostringstream os;

    bool prepend_sep {false};
    for (const auto &c : cont) {
        if (prepend_sep) {
            os << sep;
        } else {
            prepend_sep = true;
        }
        os << c;
    }
    return os.str();
}

bool starts_with(const std::string &s, const std::string &str);
bool ends_with(const std::string &s, const std::string &str);

/**
 *  print each byte value in buffer as hex string
 */
template<typename TInputIter>
std::string make_hex_str(TInputIter first, TInputIter last, bool use_uppercase=false, bool insert_spaces=true)
{
    std::ostringstream ss;
    ss << std::hex << std::setfill('0');
    if (use_uppercase) ss << std::uppercase;

    while (first != last)
    {
        ss << std::setw(2) << static_cast<uint32_t>(*first++);
        if (insert_spaces && first != last) ss << " ";
    }
    return ss.str();
}

/**
 *  print each byte value in buffer as hex string
 */
std::string make_hex_str(const uint8_t* buf, const uint32_t size, bool use_uppercase=false, bool insert_spaces=true);


} // namespace string
} // namespace util