#ifndef H_POLYNOMIAL_FORMAT
#define H_POLYNOMIAL_FORMAT

#include <initializer_list>
#include <iterator>
#include <string>

// Format a range of coefficients, starting at the coefficient of the term of
// degree zero, into a polynomial in one indeterminate, which is given by "sym".
template <typename Iter>
std::string polynomial_to_string(std::string const & sym, Iter it, Iter last)
{
    using T = typename std::iterator_traits<Iter>::value_type;
    using RI = std::reverse_iterator<Iter>;

    std::string result;
    std::size_t deg = std::distance(it, last) - 1;

    for (RI rit(last), rlast(it); rit != rlast; ++rit, --deg)
    {
        T val = *rit;

        if (val == T(0)) { continue; }

        char const * prefix = "", * mult = "";

        if (result.empty() && deg != 0)
        {
            if (val == T(-1)) { prefix = "-"; val = 1; }
        }
        else if (!result.empty())
        {
            if (val < T(0)) { prefix = " - "; val = -val; }
            else            { prefix = " + ";             }
        }

        if (deg != 0 && val != 1) { mult = " * "; }

        result += prefix;
        if (deg == 0 || val != 1) { result += std::to_string(val);       }
        result += mult;
        if (deg > 0)              { result += sym;                       }
        if (deg > 1)              { result += '^' + std::to_string(deg); }
    }

    if (result.empty()) { result = "0"; }

    return result;
}

template <typename T>
std::string polynomial_to_string(std::string const & sym, std::initializer_list<T> il)
{
    return polynomial_to_string(sym, il.begin(), il.end());
}

#endif
