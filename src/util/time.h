#pragma once

#include <chrono>
#include <iomanip>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <type_traits>

namespace util {
namespace time {

// defined in C++20
using days = std::chrono::duration<int32_t, std::ratio<86400>>;
using months  = std::chrono::duration<int32_t, std::ratio<2629746>>;

// NOLINTNEXTLINE(runtime/int)
constexpr months                    operator "" _M (unsigned long long n) { return months{n}; }
// NOLINTNEXTLINE(runtime/int)
constexpr days                      operator "" _d (unsigned long long n) { return days{n}; }
// NOLINTNEXTLINE(runtime/int)
constexpr std::chrono::hours        operator "" _h (unsigned long long n) { return std::chrono::hours{n}; }
// NOLINTNEXTLINE(runtime/int)
constexpr std::chrono::minutes      operator "" _m (unsigned long long n) { return std::chrono::minutes{n}; }
// NOLINTNEXTLINE(runtime/int)
constexpr std::chrono::seconds      operator "" _s (unsigned long long n) { return std::chrono::seconds{n}; }
// NOLINTNEXTLINE(runtime/int)
constexpr std::chrono::milliseconds operator "" _ms(unsigned long long n) { return std::chrono::milliseconds{n}; }
// NOLINTNEXTLINE(runtime/int)
constexpr std::chrono::nanoseconds  operator "" _ns(unsigned long long n) { return std::chrono::nanoseconds{n}; }

std::string to_plain_stringX(const std::tm& t);
std::string to_plain_string(const std::tm& t);
std::string to_plain_string(const std::tm& t, const std::chrono::milliseconds &ms);
std::string to_rfc3339_string(const std::tm& t);
std::string to_rfc3339_string(const std::tm& t, const std::chrono::milliseconds &ms);
std::string to_string(const std::time_t &t);
std::string to_string(const std::time_t &t, const std::chrono::milliseconds &ms);
inline std::string to_string(const std::chrono::system_clock::time_point &t)
{
    const auto ms {std::chrono::duration_cast<std::chrono::milliseconds>(t.time_since_epoch() % 1_s)};
    // If ms is negative it means past from epoch so add a second to "ms" that is negative to represent elapsed time
    // since last second.
    return to_string(std::chrono::system_clock::to_time_t(t), ms < 0_ms ? 1_s + ms : ms);
}

template <class Rep, class Period>
std::string to_string(const std::chrono::duration<Rep, Period> &dur)
{
    using std::chrono::hours;
    using std::chrono::minutes;
    using std::chrono::seconds;
    using std::chrono::milliseconds;
    using std::chrono::nanoseconds;
    using std::chrono::duration_cast;

    const auto d  {duration_cast<days>          (dur).count()};
    const auto h  {duration_cast<hours>         (dur % 1_d).count()};
    const auto m  {duration_cast<minutes>       (dur % 1_h).count()};
    const auto s  {duration_cast<seconds>       (dur % 1_m).count()};
    const auto ms {duration_cast<milliseconds>  (dur % 1_s).count()};
    const auto ns {duration_cast<nanoseconds>   (dur % 1_ms).count()};

    std::ostringstream os;

    if (d) os << d << 'D';
    if (!(h || m || s || ms || ns)) return os.str();

    os.fill('0');
    os.width(2);
    if (h) os << h << ':';
    else if (os.tellp()) os << 0 << ':';

    os.width(2);
    if (m) os << m << ':';
    else if (os.tellp()) os << 0 << ':';

    os << std::setw(os.tellp() ? 2 : 0) << s;
    if (ms || ns) {
        os << '.' << std::setw(3) << ms;
        if (ns) os << std::setw(6) << ns;
    }
    os << 's';

    return os.str();
}

inline std::string GetCurrentRfc3339Time()
{
    return util::time::to_string(std::chrono::system_clock::now());
}

inline std::chrono::minutes GetTimeZoneOffset()
{
    return -1 * std::chrono::duration_cast<std::chrono::minutes>(std::chrono::seconds{::timezone});  // defined in ctime
}

std::tm parse(const char* fmt, const std::string &time_string);
// parse "hh:mm" formatted string into minutes
inline std::chrono::minutes ParseHourMinute(const std::string &hours_minutes)
{
    std::tm t{parse("%R", hours_minutes)};
    return std::chrono::hours{t.tm_hour} + std::chrono::minutes{t.tm_min};
}

std::string GetUtcTime(const std::string& time_string);

uint64_t GetTickCount();

template <class T>
struct is_time_unit : std::false_type {};
template <class Rep, typename Period>
struct is_time_unit<std::chrono::duration<Rep, Period>> : std::true_type {};

template <class From, class To>
typename std::enable_if<is_time_unit<To>::value, To>::type
duration_convert(const uint64_t &t) {
    return std::chrono::duration_cast<To>(typename std::enable_if<is_time_unit<From>::value, From>::type{t});
}

template<class time_unit>
typename std::enable_if<is_time_unit<time_unit>::value, time_unit>::type
ParseTimeUnitTo(const std::string &time_expr)
{
    using std::chrono::duration_cast;
    using std::chrono::hours;
    using std::chrono::minutes;
    using std::chrono::seconds;
    using std::chrono::milliseconds;

    static std::map<std::string, std::function<time_unit(const uint64_t &t)>> unit_converters {
        {"M",  duration_convert<months, time_unit>},
        {"d",  duration_convert<days, time_unit>},
        {"h",  duration_convert<hours, time_unit>},
        {"m",  duration_convert<minutes, time_unit>},
        {"s",  duration_convert<seconds, time_unit>},
        {"ms", duration_convert<milliseconds, time_unit>},
    };
    std::smatch match;
    std::stringstream regex;
    regex << R"(^(\d+)()";
    for (const auto &c : unit_converters) {
        regex << c.first << "|";
    }
    regex.seekp(-1, std::ios_base::end) << ")$";

    if (std::regex_match(time_expr, match, std::regex{regex.str()})) {
        const auto converter {unit_converters[match[2]]};
        const auto t {std::stoull(match[1].str())};
        return converter(t);
    }
    return time_unit{};
}

} // namespace time
} // namespace util
