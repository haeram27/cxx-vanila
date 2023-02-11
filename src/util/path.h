#pragma once

#include <string>
#include <list>
#include <initializer_list>
#include <cstdio>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

namespace util {
namespace path {

constexpr char sep {'/'};
constexpr size_t max_length { FILENAME_MAX /* used to 4096 */ };

std::string MakePath(std::initializer_list<const std::string> args, const bool absolute = false);
bool is_file(const std::string &file_path);
bool is_dir(const std::string& dir_path);
bool is_executable(const std::string& file_path);
bool is_symlink(const std::string& symlink);
std::string filename(const std::string& file_path);
std::string dirname(const std::string& file_path);
bool remove_file(const std::string& file_path);
bool mkdir(const std::string& dirpath, const mode_t mode = 0777);
bool rmdir(const std::string& dirpath);
std::string getcwd();
std::list<dirent> readdir(const std::string& dirpath);
std::list<std::string> lsdir(const std::string& dirpath);
std::string readlink(const std::string &symlink);

inline std::string SelfExeName() {
    return filename(readlink("/proc/self/exe"));
}

} // namespace path
} // namespace util
