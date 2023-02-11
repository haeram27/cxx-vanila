#pragma once

#include <string>
#include <list>
#include <utility>
#include <cstdint>

namespace util {
namespace net {
namespace firewall {

bool open_tcp_port(uint16_t number);

} // namespace firewall
} // namespace net
} // namespace util
