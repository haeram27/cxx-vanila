#include <utility>
#include <thread>
#include <string>
#include <fstream>
#include <regex>
#include <system_error>
#include <signal.h>
#include <unistd.h>
#include "string.h"
#include "time.h"
#include "process.h"

namespace util {
namespace process {

static constexpr char proc_root[] {"/proc"};

static decltype(util::path::lsdir({})) proc_pids()
{
    const std::string proc_dir {proc_root};
    auto proc_entries {util::path::lsdir(proc_dir)};
    proc_entries.remove_if([](std::string &name) {
        return !regex_match(name, std::regex{R"(^\d+$)"});
    });

    return proc_entries;
}

static inline std::string procentry(const std::string &pid, const std::string &entry)
{
    return util::path::MakePath({proc_root, pid, entry});
}

static inline std::ifstream procstream(const std::string &pid, const std::string &entry)
{
    return std::ifstream {procentry(pid, entry)};
}

static inline std::ifstream procstream(const pid_t pid, const std::string &entry)
{
    return std::ifstream {procentry(std::to_string(pid), entry)};
}

static inline std::string nameof(const std::string &pid)
{
    std::string name;
    procstream(pid, "comm") >> name;
    return name;
}

static inline std::string cmdof(const std::string &pid)
{
    std::string argv0;
    std::getline(procstream(pid, "cmdline"), argv0, static_cast<char>(0x00));
    return argv0;
}

static inline std::string execof(const std::string &pid)
{
    decltype(execof({})) path;
    try {
        path = util::path::readlink(procentry(pid, "exe"));
    } catch (const std::system_error &e) {
        // This could be happen if the proc entry is about kernel thread or thread, its parent is already terminated.
        if (e.code().value() != ENOENT) throw e;
    }
    return path;
}

[[noreturn]] void execve(const std::string &filename, const argv_t &argv, char *const envp[])
{
    if (!util::path::is_executable(filename)) {
        throw std::invalid_argument {filename +  "is not an executable"};
    }

    auto argv_ {argv};
    argv_.push_back(nullptr);

    if (::execve(filename.c_str(), const_cast<char* const *>(&argv_[0]), envp) == -1) {
        throw std::runtime_error {"Failed to EXECUTE"};
    }

    throw std::runtime_error {"This MUST never be reached"};
}

[[noreturn]] void execve(const std::string &filename, const argv_t &argv, const argv_t &envp)
{
    auto envp_ {envp};
    envp_.push_back(nullptr);

    execve(filename, argv, const_cast<char* const *>(&envp_[0]));
}

static void set_stdio(const int stdin, const int stdout, const int stderr)
{
    if (stdin < 0) ::close(STDIN_FILENO);
    else if (stdin  != STDIN_FILENO)  dup2(stdin, STDIN_FILENO);

    if (stdout < 0) ::close(STDOUT_FILENO);
    else if (stdout != STDOUT_FILENO) dup2(stdout, STDOUT_FILENO);

    if (stderr < 0) ::close(STDERR_FILENO);
    else if (stderr != STDERR_FILENO) dup2(stderr, STDERR_FILENO);
}

pid_t execute(const int stdin, const int stdout, const int stderr, const std::string &filename, const argv_t &argv)
{
    if (!util::path::is_executable(filename)) {
        throw std::invalid_argument {filename +  "is not an executable"};
    }

    const auto pid {fork()};
    if (pid == 0) {
        set_stdio(stdin, stdout, stderr);
        execve(filename, argv.size() ? argv : argv_t {filename.c_str()});
    } else if (pid < 0) {
        throw std::runtime_error {util::err::strerror("Failed to fork a new process")};
    }

    return pid;
}

int wait(const pid_t pid)
{
    if (pid <= 0) {
        throw std::invalid_argument{"pid should be positive to specify exact one process : " + std::to_string(pid)};
    }

    int status;
    if (::waitpid(pid, &status, 0) == -1) {
        throw std::runtime_error{util::err::strerror("Process(" + std::to_string(pid) + ") is not waitable")};
    }

    if (!WIFEXITED(status)) {
        // FIXME more granual error info should be offered
        throw std::runtime_error {
            "Process(" + std::to_string(pid) + ") is not terminated normally : " + std::to_string(status)
        };
    }

    return WEXITSTATUS(status);
}

int wait_for(const pid_t pid, const std::chrono::nanoseconds &d)
{
    using std::invalid_argument;
    using std::runtime_error;
    using std::to_string;
    using std::chrono::steady_clock;
    using std::chrono::duration_cast;
    using std::chrono::seconds;
    using util::time::operator""_s;

    if (pid <= 0) {
        throw invalid_argument{"pid should be positive to specify exact one process : " + to_string(pid)};
    } else if (d < d.zero()) {
        throw invalid_argument{"duration must positive for waiting process termiation : " + to_string(pid)};
    }

    sigset_t sigchld, sigold;
    if (sigemptyset(&sigchld) || sigaddset(&sigchld, SIGCHLD) || sigprocmask(SIG_BLOCK, &sigchld, &sigold)) {
        throw runtime_error {util::err::strerror("Failed to prepare to wait SIGCHLD")};
    }

    int wait_result {0};
    siginfo_t info; info.si_pid = 0;
    const auto begin = steady_clock::now();
    while (info.si_pid != pid && !wait_result) {
        const auto time_left {d - (steady_clock::now() - begin)};
        if (time_left <= d.zero()) {
            wait_result = -1;
        } else {
            const timespec timeout {
                .tv_sec = duration_cast<seconds>(time_left).count(),
                .tv_nsec = (time_left % 1_s).count()
            };
            wait_result = sigtimedwait(&sigchld, &info, &timeout);
        }
    }

    const auto is_member {sigismember(&sigold, SIGCHLD)};
    if (!is_member) {
        sigprocmask(SIG_UNBLOCK, &sigchld, nullptr);
    } else if (is_member < 0) {
        throw std::runtime_error {"Failed to check SIGCHLD signal"};
    }

    if (wait_result < 0) {
        throw timeout_error {pid, d};
    } else if (wait_result != SIGCHLD) {
        throw runtime_error {"Unexpected signal (not SIGCHLD) is got : " + to_string(wait_result)};
    }

    return wait(pid);
}

bool exists(const pid_t pid)
{
    if (pid <= 0) throw std::invalid_argument {"pid to check existence is not positive :" + std::to_string(pid)};

    const std::string proc_entry{util::path::MakePath({proc_root, std::to_string(pid)})};
    return util::path::is_dir(proc_entry);
}

pid_set_t pidof(const std::string &process_name)
{
    using util::path::filename;
    using util::path::MakePath;

    decltype(pidof({})) pids;
    for (const auto &pid : proc_pids()) {
        if (nameof(pid) == process_name) pids.insert(std::stoi(pid));
    }

    return pids;
}

pid_set_t pidofcmd(const std::string &command, const bool fullpath)
{
    decltype(pidofcmd({}, true)) pids;
    for (const auto &pid : proc_pids()) {
        const std::string argv0 {cmdof(pid)};

        if ((fullpath ? argv0 : util::path::filename(argv0)) == command) {
            pids.insert(std::stoi(pid));
        }
    }

    return pids;
}

pid_set_t pidofexe(const std::string &full_exec_path)
{
    decltype(pidofexe({})) pids;
    for (const auto &pid : proc_pids()) {
        try {
            if (full_exec_path == execof(pid)) {
                pids.insert(std::stoi(pid));
            }
        } catch (const std::system_error &e) {
            if (e.code().value() != EACCES) throw e;
        }
    }

    return pids;
}

pid_t ppidof(const pid_t pid)
{
    if (pid == getpid()) return ::getppid();

    pid_t _pid, ppid;
    std::string comm;
    char state;

    procstream(pid, "stat") >> _pid >> comm >> state >> ppid;
    return ppid;
}

std::string nameof(const pid_t pid)
{
    return nameof(std::to_string(pid));
}

std::string cmdof(const pid_t pid)
{
    return cmdof(std::to_string(pid));
}

std::string execof(const pid_t pid)
{
    return execof(std::to_string(pid));
}

bool kill_pstree(const pid_t root_pid)
{
    if (::kill(root_pid, SIGSTOP)) {
        throw std::runtime_error {
            util::err::strerror("Failed to stop root process to kill :" + std::to_string(root_pid))};
    }

    std::multimap<pid_t, pid_t> childpids;
    for (const auto &pid_str : proc_pids()) {
        const auto &pid {std::stoi(pid_str)};
        childpids.emplace(ppidof(pid), pid);
    }

    std::list<pid_t> pids_tokill{root_pid};
    for (auto i = pids_tokill.rbegin(); i != pids_tokill.rend(); ++i) {
        const auto child_pids = childpids.equal_range(*i);
        for (auto j {child_pids.first}; j != child_pids.second; ++j) {
            pids_tokill.push_front(j->second);
        }
    }

    bool killed {true};
    for (const auto &pid : pids_tokill) {
        if (::kill(pid, SIGKILL)) killed = false;
    }

    return killed;
}

} // namespace process
} // namespace util