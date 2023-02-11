#include <cerrno>
#include <memory>
#include <regex>
#include <system_error>
#include <libgen.h>
#include <unistd.h>
#include <dirent.h>
#include "err.h"
#include "path.h"

using namespace util::path;

namespace util {
namespace path {

std::string MakePath(std::initializer_list<const std::string> args, const bool absolute)
{
    std::string path;
    if (args.size() > 0) {
        for (const auto &a : args) {
            path.append(a.c_str()); // FIXME : what if "a" contains invalid character?
            path.append(1, sep);
        }
        path.erase(path.end() - 1);
    }

    if (absolute && path[0] != '/') {
        return sep + path;
    }

    return path;
}

bool is_file(const std::string &file_path)
{
    return ::access(file_path.c_str(), F_OK) == 0;
}

bool is_dir(const std::string& dir_path)
{
    struct stat buf{};
    return ::stat(dir_path.c_str(), &buf) == 0 && S_ISDIR(buf.st_mode);
}

bool is_executable(const std::string& file_path)
{
    return ::access(file_path.c_str(), F_OK | X_OK) == 0;
}

bool is_symlink(const std::string& symlink)
{
    struct stat buf{};
    return ::lstat(symlink.c_str(), &buf) == 0 && S_ISLNK(buf.st_mode);
}

std::string filename(const std::string& file_path)
{
    std::vector<char> buf {file_path.cbegin(), file_path.cend()};
    buf.push_back(0x00);
    return ::basename(&buf[0]);
}

std::string dirname(const std::string& file_path)
{
    std::vector<char> buf {file_path.cbegin(), file_path.cend()};
    buf.push_back(0x00);
    return ::dirname(&buf[0]);
}

bool remove_file(const std::string& file_path)
{
    return std::remove(file_path.c_str()) == 0;
}

bool mkdir(const std::string& dirpath, const mode_t mode)
{
    if (is_dir(dirpath)) return true;

    std::vector<char> buf {dirpath.cbegin(), dirpath.cend()};
    buf.push_back(0x00);

    const auto basedir {::dirname(&buf[0])};
    return ((is_dir(basedir) || mkdir(basedir, mode)) && (::mkdir(dirpath.c_str(), mode) == 0));
}

bool rmdir(const std::string& dirpath)
{
    for (const auto &entry : readdir(dirpath)) {
        const auto target {MakePath({dirpath, entry.d_name})};
#ifndef _DIRENT_HAVE_D_TYPE
        static_assert(false, "struct dirent doesn't include d_type attribute");
#endif
        if (!(entry.d_type == DT_DIR ? rmdir(target) : remove_file(target))) {
            break;
        }
    }

    if (::rmdir(dirpath.c_str())) {
//        throw std::runtime_error{ util::err::strerror("Failed to rmdir : " + dirpath)};
        throw std::runtime_error{ "Failed to rmdir : " + dirpath };
    }

    return true;
}

std::string getcwd()
{
    const auto free = [] (char *ptr){::free(ptr);};
    std::unique_ptr<char, decltype(free)> cwd {::getcwd(nullptr, 0), free};
    if (!cwd) throw std::runtime_error {"Failed to get CWD to extract zip archive"};

    return std::string{cwd.get()};
}

std::list<dirent> readdir(const std::string& dirpath)
{
    if (!is_dir(dirpath)) throw std::invalid_argument {dirpath + " is not a directory"};
    const auto closedir = [&dirpath](DIR *d) {
        //if (::closedir(d)) throw std::runtime_error{util::err::strerror("Failed to close directory : " + dirpath)};
        if (::closedir(d)) throw std::runtime_error{"Failed to close directory : " + dirpath};
    };
    const std::unique_ptr<DIR, decltype(closedir)> dir {::opendir(dirpath.c_str()), closedir};
    //if (!dir) throw std::runtime_error{util::err::strerror("Failed to open directory : " + dirpath)};
    if (!dir) throw std::runtime_error{"Failed to open directory : " + dirpath};

    decltype(readdir({})) entries;

    while (const auto entry{::readdir(dir.get())}) {
        const auto &d_name{entry->d_name};
        if (d_name[0] == '.' && (d_name[1] == 0x00 || (d_name[1] == '.' && d_name[2] == 0x00))) continue;
        entries.emplace_back(std::move(*entry));
    }

    return entries;
}

std::list<std::string> lsdir(const std::string& dirpath)
{
    const auto dirents {readdir(dirpath)};
    decltype(lsdir({})) names {dirents.size()};
    std::transform(dirents.cbegin(), dirents.cend(), names.begin(), [](const dirent &entry) {
        return entry.d_name;
    });
    return names;
}

std::string readlink(const std::string &symlink)
{
    if (!is_symlink(symlink)) throw std::invalid_argument {"trying to read non-symbolic link: " + symlink};

    std::string buf(max_length, 0x00);

    auto len = ::readlink(symlink.c_str(), &buf[0], buf.size());
    if (len == -1) throw std::system_error{errno, std::system_category(), symlink};

    buf.resize(static_cast<decltype(buf)::size_type>(len));
    return buf;
}

} // namespace path
} // namespace util