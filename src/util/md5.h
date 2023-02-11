#pragma once

#include <algorithm>
#include <array>
#include <fstream>
#include <cstdint>
#include <cstddef>
#include <type_traits>

namespace util {
namespace hash {

class MD5 {
 public:
    constexpr static size_t hash_size = 16;
    using value_type = std::array<uint8_t, hash_size>;

    MD5();
    ~MD5() = default;

    template <class T> explicit MD5(const T& cont)     : MD5(cont.begin(), cont.end()) {}
    template <class InputIt> explicit MD5(const InputIt& cbegin, const InputIt& cend) : MD5() { update(cbegin, cend); }

    explicit MD5(const char *msg)   : MD5(std::string{msg}) {}
    explicit MD5(std::ifstream &is) : MD5(std::move(is)) {}
    explicit MD5(std::ifstream &&is);

    operator std::string&() {
        if (md5_str.empty()) final();
        return md5_str;
    }

    template <class InputIt>
    MD5 &update(InputIt begin, InputIt end) {
        if (!md5_str.empty()) throw std::logic_error {"Once finalized MD5 can't be updated"};

        auto togo {end - begin};
        auto preinput_offset {ninput()};

        nbits += static_cast<decltype(nbits)>(togo) << 3;

        const auto input_buf {reinterpret_cast<uint8_t*>(input.data())};
        if (preinput_offset) {
            const auto ip {input_buf + preinput_offset};
            preinput_offset = block_size - preinput_offset;
            if (togo < preinput_offset) {
                std::copy_n(begin, togo, ip);
                return *this;
            }
            std::copy_n(begin, preinput_offset, ip);
            round(input);
            begin += preinput_offset;
            togo -= preinput_offset;
        }

        while (togo >= block_size) {
            std::copy_n(begin, block_size, input_buf);
            round(input);
            begin += block_size;
            togo -= block_size;
        }

        std::copy_n(begin, togo, input_buf);
        return *this;
    }

    template <class T>
    MD5 &operator<<(const T &rhs) {
        update(rhs.begin(), rhs.end());
        return *this;
    }
    MD5 &operator<<(const char *rhs) { return operator<<(std::string{rhs}); }

    MD5 &operator>>(value_type &rhs) {
        if (md5_str.empty()) final();
        std::copy_n(output.cbegin(), output.size(), rhs.begin());
        return *this;
    }

    MD5 &operator>>(std::string &rhs) {
        if (md5_str.empty()) final();
        rhs = md5_str;
        return *this;
    }

 protected:
    using input_block_t = std::array<uint32_t, hash_size>;
    constexpr static ssize_t block_size {64};   // This supposed to be a size_t but ssize_t remove casting warnings.

    void round(const input_block_t &in);
    void final();

 private:
    inline input_block_t::difference_type ninput() const {
        return (nbits >> 3) & 63 /*0x3f*/;
    }

    uint64_t                nbits;  /* number of _bits_ handled mod 2^64 */
    std::array<uint32_t, 4> state;  /* scratch buffer */
    input_block_t           input;  /* input buffer */
    value_type              output;
    std::string             md5_str;
};

template<class T>
MD5 &operator>>(const T &lhs, MD5 &md5) {
    return md5 << lhs;
}

inline MD5 &operator>>(const char *lhs, MD5 &&md5) {
    return md5 << std::string{lhs};
}

} // namespace hash
} // namespace util