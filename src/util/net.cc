#include <iostream>
#include <limits>
#include <regex>
#include "net.h"


namespace util {
namespace net {
namespace uri {

#define PROTOCOL    R"((?:([^:/?#]+)://)?)"
#define USERNAME    R"(([^@:/?#]+))"
#define PASSWORD    R"((?::([^@]+))?)"
#define HOSTNAME    R"(((?:[^@:/?#]+[^\.:/]|\[[:\.0-9a-fA-F]+\])))"
#define PORTNUM     R"((?::([0-9]{1,5}))?)"
#define PATHNAME    R"((/[^?#]*)?)"
#define QUERY       R"((?:\?([^#]*))?)"
#define HASH        R"((#.*)?)"
const std::regex rex_uri{"^" PROTOCOL "(?:" USERNAME PASSWORD "@)?" HOSTNAME PORTNUM PATHNAME QUERY HASH};
#undef PROTOCOL
#undef USERNAME
#undef PASSWORD
#undef HOSTNAME
#undef PORTNUM
#undef PATHNAME
#undef QUERY
#undef HASH

URI parse(const std::string &uri)
{
    std::smatch match;

    if (!std::regex_match(uri, match, rex_uri)) {
        throw std::invalid_argument{"URI is not recognizable : " + uri};
    }
    if (match.size() != 9) {
        std::cerr << match.size() << std::endl;
        throw std::logic_error{"URI is parsed unexpected way : " + uri};
    }

    const decltype(URI::port)     port      {match[5].str()};
    if (!port.empty() && std::stoull(port) > std::numeric_limits<unsigned int>::max()) {
        throw std::invalid_argument{"URI is not recognizable : " + uri};
    }

    const decltype(URI::href)     href      {match[0].str()};
    const decltype(URI::protocol) protocol  {match[1].str()};
    const decltype(URI::username) username  {match[2].str()};
    const decltype(URI::password) password  {match[3].str()};
    const decltype(URI::hostname) raw_name  {match[4].str()};
    const decltype(URI::pathname) pathname  {match[6].str()};
    const decltype(URI::query)    query     {match[7].str()};
    const decltype(URI::hash)     hash      {match[8].str()};
    const decltype(URI::host)     host      {raw_name + (port.empty() ? "" : ":" + port)};
    const decltype(URI::hostname) hostname  {raw_name[0] == '[' ? raw_name.substr(1, raw_name.length() - 2) : raw_name};

    return URI {
        .href     = std::move(href),
        .protocol = std::move(protocol),
        .auth     = username + (password.empty() ? "" : ":" + password),
        .host     = std::move(host),
        .path     = pathname + (query.empty() ? "" : "?" + query),
        .hash     = std::move(hash),
        .hostname = std::move(hostname),
        .port     = std::move(port),
        .pathname = std::move(pathname),
        .search   = query.empty() ? "" : "?" + query,
        .query    = std::move(query),
        .username = std::move(username),
        .password = std::move(password),
        .origin   = (protocol.empty() ? "" : protocol + "://") + host,
    };
}

} // namespace uri
} // namespace net
} // namespace util
