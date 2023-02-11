#pragma once

#include <type_traits>
#include <iterator>

namespace util {
namespace type {

template<class T>
struct is_random_access_container : std::is_same<
    std::random_access_iterator_tag,
    typename std::iterator_traits<typename T::iterator>::iterator_category
>{};

template<class T>
using random_access_container = typename std::enable_if<is_random_access_container<T>::value, T>::type;

template<class T>
using not_random_access_container = typename std::enable_if<!is_random_access_container<T>::value, T>::type;
} // namespace type
} // namespace util