#pragma once

#include <string>
#include <vector>
#include <set>
#include <chrono>
#include <stdexcept>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "string.h"
#include "path.h"
#include "err.h"
#include "type.h"
#include "time.h"

namespace util {
namespace process {

using argv_t = std::vector<char const *>;

[[noreturn]] void execve(const std::string &filename, const argv_t &argv, char *const envp[] = environ);
[[noreturn]] void execve(const std::string &filename, const argv_t &argv, const argv_t &envp);

// XXX This is simple wrapper of fork/exec. Caller MUST prevent child process not to be zombie.
pid_t execute(
        const int stdin,
        const int stdout,
        const int stderr,
        const std::string &filename,
        const argv_t &argv = {});

template <class... Args>
inline pid_t execute(
        const int stdin,
        const int stdout,
        const int stderr,
        const std::string &filename,
        const Args& ...args)
{
    return execute(
            stdin,
            stdout,
            stderr,
            filename,
            { filename.c_str(), (util::string::tostring(args).c_str())... });
}

template <class... Args>
inline pid_t execute(const std::string &filename, const Args& ...args)
{
    return execute(STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO, filename, args...);
}

class timeout_error : public std::runtime_error {
 public:
    timeout_error() = delete;
    timeout_error(const pid_t &id, const std::chrono::nanoseconds &d) : std::runtime_error(
            "process(" + std::to_string(id) + ") is not terminated normally within given duration "
            + util::time::to_string(d)),
        pid_(id), duration_(d) {}
    inline pid_t pid() const {return pid_;}
    inline std::chrono::nanoseconds duration() const {return duration_;}

 private:
    const pid_t pid_;
    const std::chrono::nanoseconds duration_;
};

int wait(const pid_t pid);
int wait_for(const pid_t pid, const std::chrono::nanoseconds &d);

bool exists(const pid_t pid);

using pid_set_t = std::set<pid_t>;
pid_set_t pidof(const std::string &process_name);
pid_set_t pidofcmd(const std::string &command, const bool fullpath = false);
pid_set_t pidofexe(const std::string &full_exec_path);

pid_t ppidof(const pid_t pid);

std::string nameof(const pid_t pid);
std::string cmdof(const pid_t pid);
std::string execof(const pid_t pid);

bool kill_pstree(const pid_t root_pid);

} // namespace process
} // namespace util
