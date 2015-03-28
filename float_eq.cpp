#include <algorithm>
#include <cmath>
#include <cstdint>
#include <type_traits>

#include "float_eq.hpp"

template <typename T, typename U>
T bitcopy(U && u)
{
    static_assert(sizeof(T) == sizeof(U),
                  "Source and destination types have different sizes.");

    static_assert(std::is_trivial<T>::value,
                  "Destination type is not trivial.");

    // (Compiler support for this trait is lacking at the moment.)
    // static_assert(std::is_trivially_copyable<std::decay<U>::type>::value,
    //               "Source type is not trivially copyable.");

    T t;
    std::copy_n(reinterpret_cast<unsigned char const *>(&u),   // from
                sizeof(T),                                     // how much
                reinterpret_cast<unsigned char *>(&t));        // to
    return t;
}

bool float_eq(double a, double b, std::uint64_t ulp, std::uint64_t * ulpdiff)
{
    if (ulpdiff) { *ulpdiff = 0; }

    // Handle NaN.
    if (std::isunordered(a, b)) { return false; }

    // Handle +/- zero.
    if (std::signbit(a) != std::signbit(b)) { return a == b; }

    // Handle infinity (by now, a and b must have the same sign).
    if (std::isinf(a) && std::isinf(b)) { return true; }

    auto ia = bitcopy<std::uint64_t>(a), ib = bitcopy<std::uint64_t>(b);
    std::uint64_t diff = ia < ib ? ib - ia : ia - ib;
    if (ulpdiff) { *ulpdiff = diff; }
    return diff < ulp;
}
