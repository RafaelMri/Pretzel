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
    std::string result;

    if (it == last) { result = "0"; return result; }

    using T = typename std::iterator_traits<Iter>::value_type;
    using RI = std::reverse_iterator<Iter>;

    std::size_t deg = std::distance(it, last) - 1;

    for (RI rit(last), rlast(it); rit != rlast; ++rit, --deg)
    {
        if (*rit == 0) { continue; }

        if (result.empty())
        {
            if (deg == 0)               {                  result += std::to_string(*rit); }
            else if (deg == 1)
            {
                if (*rit == T(1))       {                                                                    result += sym; }
                else if (*rit == T(-1)) { result += '-';                                                     result += sym; }
                else                    {                  result += std::to_string(*rit);  result += " * "; result += sym; }
            }
            else
            {
                if (*rit == T(1))       {                                                                    result += sym; result += '^'; result += std::to_string(deg); }
                else if (*rit == T(-1)) { result += '-';                                                     result += sym; result += '^'; result += std::to_string(deg); }
                else                    {                  result += std::to_string(*rit);  result += " * "; result += sym; result += '^'; result += std::to_string(deg); }
            }
        }
        else
        {
            if (deg == 0)
            {
                if (*rit > T(0))        { result += " + "; result += std::to_string(+*rit); }
                else                    { result += " - "; result += std::to_string(-*rit); }
            }
            else if (deg == 1)
            {
                if      (*rit == T(+1)) { result += " + ";                                                   result += sym; }
                else if (*rit == T(-1)) { result += " - ";                                                   result += sym; }
                else if (*rit > T(0))   { result += " + "; result += std::to_string(+*rit); result += " * "; result += sym; }
                else                    { result += " - "; result += std::to_string(-*rit); result += " * "; result += sym; }
            }
            else
            {
                if (*rit == T(1))       { result += " + ";                                                   result += sym; result += '^'; result += std::to_string(deg); }
                else if (*rit == T(-1)) { result += " - ";                                                   result += sym; result += '^'; result += std::to_string(deg); }
                else if (*rit > T(0))   { result += " + "; result += std::to_string(+*rit); result += " * "; result += sym; result += '^'; result += std::to_string(deg); }
                else                    { result += " - "; result += std::to_string(-*rit); result += " * "; result += sym; result += '^'; result += std::to_string(deg); }
            }
        }
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
