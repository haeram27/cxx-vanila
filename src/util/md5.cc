#include <fstream>
#include <cstdint>
#include <cstring>
#include "md5.h"
#include "string.h"

namespace util {
namespace hash {

/* F, G, H and I are basic MD5 functions */
template<class T> inline static T F(const T x, const T y, const T z) {
    return ((y ^ z) & x) ^ z;
}

template<class T> inline static T G(const T x, const T y, const T z) {
    return ((x ^ y) & z) ^ y;
}

template<class T> inline static T H(const T x, const T y, const T z) {
    return (x ^ y ^ z);
}

template<class T> inline static T I(const T x, const T y, const T z) {
    return (x | (~z)) ^ y;
}

template<class T> inline static T rotleft(const T x, const T n) {
    return (x << n) | (x >> ((sizeof(T) >> 3) - n));
}

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4 */
/* Rotation is separate from addition to prevent recomputation */
template<class T>
inline static void FF(T &a, const T b, const T c, const T d, const T x, const T s, const T ac) {
    a = rotleft(a + F(b, c, d) + x + ac, s) + b;
}

template<class T>
inline static void GG(T &a, const T b, const T c, const T d, const T x, const T s, const T ac) {
    a = rotleft(a + G(b, c, d) + x + ac, s) + b;
}

template<class T>
inline static void HH(T &a, const T b, const T c, const T d, const T x, const T s, const T ac) {
    a = rotleft(a + H(b, c, d) + x + ac, s) + b;
}

template<class T>
inline static void II(T &a, const T b, const T c, const T d, const T x, const T s, const T ac) {
    a = rotleft(a + I(b, c, d) + x + ac, s) + b;
}

constexpr auto  INIT_A { 0x67452301u },
                INIT_B { 0xefcdab89u },
                INIT_C { 0x98badcfeu },
                INIT_D { 0x10325476u };

MD5::MD5() : nbits(0), state({INIT_A, INIT_B, INIT_C, INIT_D})
{
}

MD5::MD5(std::ifstream &&is): MD5()
{
    if (!is) throw std::invalid_argument {"invalid ifstream for MD5"};

    while (is) {
        std::string buf(BUFSIZ, '\0');
        is.read(&buf[0], static_cast<std::streamsize>(buf.size()));
        buf.resize(static_cast<decltype(buf)::size_type>(is.gcount()));
        update(buf.begin(), buf.end());
    }
}

void MD5::round(const input_block_t &in)
{
    constexpr uint32_t  V11 {7}, V12 {12}, V13 {17}, V14 {22},
                        V21 {5}, V22 {9},  V23 {14}, V24 {20},
                        V31 {4}, V32 {11}, V33 {16}, V34 {23},
                        V41 {6}, V42 {10}, V43 {15}, V44 {21};

    auto a {state[0]},
         b {state[1]},
         c {state[2]},
         d {state[3]};

    /* MD5_Round 1 */
    FF(a, b, c, d, in[ 0], V11, 0xd76aa478u); /* 1 */
    FF(d, a, b, c, in[ 1], V12, 0xe8c7b756u); /* 2 */
    FF(c, d, a, b, in[ 2], V13, 0x242070dbu); /* 3 */
    FF(b, c, d, a, in[ 3], V14, 0xc1bdceeeu); /* 4 */
    FF(a, b, c, d, in[ 4], V11, 0xf57c0fafu); /* 5 */
    FF(d, a, b, c, in[ 5], V12, 0x4787c62au); /* 6 */
    FF(c, d, a, b, in[ 6], V13, 0xa8304613u); /* 7 */
    FF(b, c, d, a, in[ 7], V14, 0xfd469501u); /* 8 */
    FF(a, b, c, d, in[ 8], V11, 0x698098d8u); /* 9 */
    FF(d, a, b, c, in[ 9], V12, 0x8b44f7afu); /* 10 */
    FF(c, d, a, b, in[10], V13, 0xffff5bb1u); /* 11 */
    FF(b, c, d, a, in[11], V14, 0x895cd7beu); /* 12 */
    FF(a, b, c, d, in[12], V11, 0x6b901122u); /* 13 */
    FF(d, a, b, c, in[13], V12, 0xfd987193u); /* 14 */
    FF(c, d, a, b, in[14], V13, 0xa679438eu); /* 15 */
    FF(b, c, d, a, in[15], V14, 0x49b40821u); /* 16 */

    /* MD5_Round 2 */
    GG(a, b, c, d, in[ 1], V21, 0xf61e2562u); /* 17 */
    GG(d, a, b, c, in[ 6], V22, 0xc040b340u); /* 18 */
    GG(c, d, a, b, in[11], V23, 0x265e5a51u); /* 19 */
    GG(b, c, d, a, in[ 0], V24, 0xe9b6c7aau); /* 20 */
    GG(a, b, c, d, in[ 5], V21, 0xd62f105du); /* 21 */
    GG(d, a, b, c, in[10], V22, 0x02441453u); /* 22 */
    GG(c, d, a, b, in[15], V23, 0xd8a1e681u); /* 23 */
    GG(b, c, d, a, in[ 4], V24, 0xe7d3fbc8u); /* 24 */
    GG(a, b, c, d, in[ 9], V21, 0x21e1cde6u); /* 25 */
    GG(d, a, b, c, in[14], V22, 0xc33707d6u); /* 26 */
    GG(c, d, a, b, in[ 3], V23, 0xf4d50d87u); /* 27 */
    GG(b, c, d, a, in[ 8], V24, 0x455a14edu); /* 28 */
    GG(a, b, c, d, in[13], V21, 0xa9e3e905u); /* 29 */
    GG(d, a, b, c, in[ 2], V22, 0xfcefa3f8u); /* 30 */
    GG(c, d, a, b, in[ 7], V23, 0x676f02d9u); /* 31 */
    GG(b, c, d, a, in[12], V24, 0x8d2a4c8au); /* 32 */

    /* MD5_Round 3 */
    HH(a, b, c, d, in[ 5], V31, 0xfffa3942u); /* 33 */
    HH(d, a, b, c, in[ 8], V32, 0x8771f681u); /* 34 */
    HH(c, d, a, b, in[11], V33, 0x6d9d6122u); /* 35 */
    HH(b, c, d, a, in[14], V34, 0xfde5380cu); /* 36 */
    HH(a, b, c, d, in[ 1], V31, 0xa4beea44u); /* 37 */
    HH(d, a, b, c, in[ 4], V32, 0x4bdecfa9u); /* 38 */
    HH(c, d, a, b, in[ 7], V33, 0xf6bb4b60u); /* 39 */
    HH(b, c, d, a, in[10], V34, 0xbebfbc70u); /* 40 */
    HH(a, b, c, d, in[13], V31, 0x289b7ec6u); /* 41 */
    HH(d, a, b, c, in[ 0], V32, 0xeaa127fau); /* 42 */
    HH(c, d, a, b, in[ 3], V33, 0xd4ef3085u); /* 43 */
    HH(b, c, d, a, in[ 6], V34, 0x04881d05u); /* 44 */
    HH(a, b, c, d, in[ 9], V31, 0xd9d4d039u); /* 45 */
    HH(d, a, b, c, in[12], V32, 0xe6db99e5u); /* 46 */
    HH(c, d, a, b, in[15], V33, 0x1fa27cf8u); /* 47 */
    HH(b, c, d, a, in[ 2], V34, 0xc4ac5665u); /* 48 */

    /* MD5_Round 4 */
    II(a, b, c, d, in[ 0], V41, 0xf4292244u); /* 49 */
    II(d, a, b, c, in[ 7], V42, 0x432aff97u); /* 50 */
    II(c, d, a, b, in[14], V43, 0xab9423a7u); /* 51 */
    II(b, c, d, a, in[ 5], V44, 0xfc93a039u); /* 52 */
    II(a, b, c, d, in[12], V41, 0x655b59c3u); /* 53 */
    II(d, a, b, c, in[ 3], V42, 0x8f0ccc92u); /* 54 */
    II(c, d, a, b, in[10], V43, 0xffeff47du); /* 55 */
    II(b, c, d, a, in[ 1], V44, 0x85845dd1u); /* 56 */
    II(a, b, c, d, in[ 8], V41, 0x6fa87e4fu); /* 57 */
    II(d, a, b, c, in[15], V42, 0xfe2ce6e0u); /* 58 */
    II(c, d, a, b, in[ 6], V43, 0xa3014314u); /* 59 */
    II(b, c, d, a, in[13], V44, 0x4e0811a1u); /* 60 */
    II(a, b, c, d, in[ 4], V41, 0xf7537e82u); /* 61 */
    II(d, a, b, c, in[11], V42, 0xbd3af235u); /* 62 */
    II(c, d, a, b, in[ 2], V43, 0x2ad7d2bbu); /* 63 */
    II(b, c, d, a, in[ 9], V44, 0xeb86d391u); /* 64 */

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
}

void MD5::final()
{
    static constexpr input_block_t PADDING { 0x80, 0x00, };

    input_block_t in;
    *reinterpret_cast<decltype(nbits)*>(&*++in.rbegin()) = nbits;

    const auto padLen {(ninput() < 56 ? 56 : 120) - ninput()};
    const auto pPadding {reinterpret_cast<uint8_t const *>(PADDING.data())};
    update(pPadding, pPadding + padLen);

    const auto &ip {reinterpret_cast<uint8_t*>(input.data())};
    for (input_block_t::size_type i = 0, j = 0; i < 14; i++, j += 4)
        in[i] = static_cast<input_block_t::value_type>(ip[j+3]) << 24 |
                static_cast<input_block_t::value_type>(ip[j+2]) << 16 |
                static_cast<input_block_t::value_type>(ip[j+1]) << 8  |
                static_cast<input_block_t::value_type>(ip[j]);
    round(in);

    for (input_block_t::size_type i = 0, j = 0; i < 4; i++, j += 4) {
        const auto &s {state[i]};
        output[j]   = static_cast<decltype(output)::value_type>(s);
        output[j+1] = static_cast<decltype(output)::value_type>(s >> 8);
        output[j+2] = static_cast<decltype(output)::value_type>(s >> 16);
        output[j+3] = static_cast<decltype(output)::value_type>(s >> 24);
    }

    md5_str = util::string::to_string(output.begin(), output.end(), 0x00);
}

} // namespace hash
} // namespace util
