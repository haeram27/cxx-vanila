#pragma once

#include <string>
#include <cstddef>

namespace util {
namespace net {

struct URI {
// https://nodejs.org/docs/latest/api/url.html#url_url_strings_and_url_objects
// +------------------------------------------------------------------------------------------------+
// |                                              href                                              |
// +----------+--+---------------------+------------------------+---------------------------+-------+
// | protocol |  |        auth         |          host          |           path            | hash  |
// |          |  |                     -----------------------------------------------------+       |
// |          |  |                     |    hostname     | port | pathname |     search     |       |
// |          |  |                     |                 |      |          +-+--------------+       |
// |          |  |                     |                 |      |          | |    query     |       |
// "  https:   //    user   :   pass   @ sub.example.com : 8080   /p/a/t/h  ?  query=string   #hash "
// |          |  |          |          |    hostname     | port |          |                |       |
// |          |  |          |          +------------------------+          |                |       |
// | protocol |  | username | password |          host          |          |                |       |
// +-------------+-----------------------------------------------          |                |       |
// |   origin    |                     |         origin         | pathname |     search     | hash  |
// +-------------+---------------------+------------------------+----------+----------------+-------+
// |                                              href                                              |
// +------------------------------------------------------------------------------------------------+
// (all spaces in the "" line should be ignored ? they are purely for formatting)
    std::string href;
    std::string protocol;
    std::string auth;
    std::string host;
    std::string path;
    std::string hash;
    std::string hostname;
    std::string port;
    std::string pathname;
    std::string search;
    std::string query;
    std::string username;
    std::string password;
    std::string origin;
};

namespace uri {

URI parse(const std::string &uri);

} // namespace uri

namespace ipv4 {

constexpr char addr_self[] {"0.0.0.0"};
constexpr char addr_broadcast[] {"255.255.255.255"};

constexpr size_t min_addr_len {sizeof(util::net::ipv4::addr_self)/sizeof(util::net::ipv4::addr_self[0])};
constexpr size_t max_addr_len {sizeof(util::net::ipv4::addr_broadcast)/sizeof(util::net::ipv4::addr_broadcast[0])};

} // namespace ipv4
} // namespace net
} // namespace util
