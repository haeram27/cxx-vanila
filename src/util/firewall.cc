#include "firewall.h"
#include "path.h"
#include "process.h"
#include <iostream>

namespace util {
namespace net {
namespace firewall {


const char *iptables {[] {
    static constexpr const char * const iptables_cmd[] {
        "/sbin/iptables",
        "/usr/sbin/iptables",
    };

    return *std::find_if(iptables_cmd,
            iptables_cmd + sizeof(iptables_cmd)/sizeof(iptables_cmd[0]),
            util::path::is_executable);
}()};

bool open_tcp_port(uint16_t number)
{
    if (!iptables) { return false; }

    try {
        using util::process::argv_t;
        using util::process::execute;
        using util::process::wait;
        return  0 == wait(execute(-1, -1, -1, iptables, "-t", "filter",
                    "-C", "INPUT", "-p", "tcp", "--dport", number, "-j", "ACCEPT")) ||
                0 == wait(execute(-1, -1, -1, iptables, "-t", "filter",
                    "-A", "INPUT", "-p", "tcp", "--dport", number, "-j", "ACCEPT"));
    } catch(...) { }

    return false;
}


} // namespace firewall
} // namespace net
} // namespace util