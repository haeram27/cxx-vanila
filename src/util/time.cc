#include <chrono>
#include <iomanip>
#include <regex>
#include <sstream>
#include <stdexcept>
#include "time.h"

using namespace std::chrono;
using util::time::operator""_s;
using util::time::operator""_ms;

namespace util {
namespace time {

inline static decltype(std::put_time(nullptr, "")) put_rfc3339_time(const std::tm *tmb)
{
    return std::put_time(tmb, "%Y-%m-%dT%T");
}

static std::ostream &operator<<(std::ostream &os, milliseconds ms)
{
    if (ms < 0_ms || 1_s <= ms)  {
        throw std::invalid_argument{"milliseconds should be between 0 and 1000 : " + std::to_string(ms.count())};
    }
    const auto width {os.width()};
    const auto fill {os.fill()};
    os << '.' << std::setw(3) << std::setfill('0') << ms.count();
    os.width(width);
    os.fill(fill);

    return os;
}

static const std::map<const std::string, std::function<std::tm(const std::string &str)>> fallback_parsers
{
    {"%R", [](const std::string &str) {
        // Without leading zero(s), it can't be parsed as a bug of gcc.
        // --> https://gcc.gnu.org/bugzilla/show_bug.cgi?id=45896
        std::smatch match;
        if (!std::regex_match(str, match, std::regex{R"(([0-1]?\d|2[0-3]):([0-5]?\d))"}))
            throw std::invalid_argument{"Failed to parse " + str};

        std::tm t{};
        t.tm_hour = std::stoi(match[1].str());
        t.tm_min  = std::stoi(match[2].str());
        return t;
    }},
};

std::string to_plain_stringX(const std::tm& t)
{
    char plain[FILENAME_MAX] = {0,};
    snprintf(plain, FILENAME_MAX, "%04u%02u%02u%02u%02u%02u",
                t.tm_year + 1900,
                t.tm_mon + 1,
                t.tm_mday,
                t.tm_hour,
                t.tm_min,
                t.tm_sec
            );
    return std::string(plain);
}

std::string to_plain_string(const std::tm& t)
{
    std::stringstream plain;

    plain << std::setfill('0') << std::setw(4) << t.tm_year + 1900;
    plain << std::setfill('0') << std::setw(2) << t.tm_mon + 1;
    plain << std::setfill('0') << std::setw(2) << t.tm_mday;
    plain << std::setfill('0') << std::setw(2) << t.tm_hour;
    plain << std::setfill('0') << std::setw(2) << t.tm_min;
    plain << std::setfill('0') << std::setw(2) << t.tm_sec;

    return plain.str();
}

std::string to_plain_string(const std::tm& t, const milliseconds &ms)
{
    std::stringstream plain;

    plain << std::setfill('0') << std::setw(4) << t.tm_year + 1900;
    plain << std::setfill('0') << std::setw(2) << t.tm_mon + 1;
    plain << std::setfill('0') << std::setw(2) << t.tm_mday;
    plain << std::setfill('0') << std::setw(2) << t.tm_hour;
    plain << std::setfill('0') << std::setw(2) << t.tm_min;
    plain << std::setfill('0') << std::setw(2) << t.tm_sec;
    plain << std::setfill('0') << std::setw(3) << ms.count();

    return plain.str();
}

std::string to_rfc3339_string(const std::tm& t)
{
    std::stringstream rfc3339_time;
    rfc3339_time << put_rfc3339_time(&t) << "Z";
    return rfc3339_time.str();
}

std::string to_rfc3339_string(const std::tm& t, const milliseconds &ms)
{
    std::stringstream rfc3339_time;
    rfc3339_time << put_rfc3339_time(&t) << ms << "Z";
    return rfc3339_time.str();
}

std::string to_string(const std::time_t &t)
{
    std::tm utc;
    if (::gmtime_r(&t, &utc) != &utc) {
        throw std::runtime_error {"Failed to convert std::time_t to std::tm"};
    }

    return to_rfc3339_string(utc);
}

std::string to_string(const std::time_t &t, const milliseconds &ms)
{
    if (ms < 0_ms || 1_s <= ms)  {
        throw std::invalid_argument{"milliseconds should be between 0 and 1000 : " + std::to_string(ms.count())};
    }

    std::tm utc;
    if (::gmtime_r(&t, &utc) != &utc) {
        throw std::runtime_error {"Failed to convert std::time_t to std::tm"};
    }

    return to_rfc3339_string(utc, ms);
}

std::tm parse(const char* fmt, const std::string &time_string)
{
    std::tm t{};
    std::istringstream is {time_string};
    is >> std::get_time(&t, fmt);
    if (!is.fail()) return t;

    auto parser{fallback_parsers.find(fmt)};
    if (parser == fallback_parsers.cend()) {
        throw std::invalid_argument{"Failed to parse time string : " + time_string};
    }
    return parser->second(time_string);
}

/// <summary>
/// 일반적인 시간 형식의 Local Time 문자열을 UTC 형식의 UTC Time 문자열로 변환한다.
/// </summary>
/// <param name="local_time_string">YYYY-MM-DD hh:mm:ss.ms 형식의 Local Time 문자열</param>
/// <returns>
/// <list type="bullet">
/// <item>UTC 형식(YYYY-MM-DDThh:mm:ss.msZ)의 UTC Time 문자열</item>
/// </list>
/// </returns>
std::string GetUtcTime(const std::string& time_string)
{
    std::tm t{};
    std::uint16_t ms;

    std::istringstream is {time_string};
    is >> std::get_time(&t, "%Y-%m-%d %T.") >> ms;
    if (is.fail() || ms >= 1000) {
        throw std::invalid_argument{"Failed to parse time string(" + time_string + ")"};
    }

    t.tm_min -= static_cast<decltype(tm::tm_min)>(GetTimeZoneOffset().count());
    if (std::mktime(&t) == -1) {
        //throw std::runtime_error{util::err::strerror("Failed to recalculate time")};
    }

    std::stringstream rfc3339_time;
    rfc3339_time << put_rfc3339_time(&t) << milliseconds{ms} << "Z";
    return rfc3339_time.str();
}

/// <summary>
/// Get time-tick-count since when system is booted.
/// </summary>
/// <returns>
/// <list type="bullet">
/// <item>uint64 size of integer (unit of milliseconds)</item>
/// </list>
/// </returns>
uint64_t GetTickCount()
{
	auto tp = steady_clock::now();
	auto du_msec = duration_cast<milliseconds>(tp.time_since_epoch());
	return du_msec.count();
}

} // namespace time
} // namespace util
