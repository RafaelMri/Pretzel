#ifndef H_MATRIX_FORMAT
#define H_MATRIX_FORMAT

#include <iomanip>
#include <iosfwd>

#include "matrix.hpp"

template <typename T>
struct matrix_printer
{
    matrix<T> const & mat;
    char const * pre;
    int wd;

    matrix_printer(matrix<T> const & m, char const * p, int w = 0)
    : mat(m), pre(p), wd(w) {}
};

template <typename T>
struct inline_matrix_printer
{
    matrix<T> const & mat;
    char const * outer_sep;
    char const * inner_sep;

    inline_matrix_printer(matrix<T> const & m, char const * outer, char const * inner)
    : mat(m), outer_sep(outer), inner_sep(inner) {}
};

template <typename T>
matrix_printer<T> print(matrix<T> const & m, char const * p, std::size_t w)
{
    return matrix_printer<T>(m, p, w);
}

template <typename T>
inline_matrix_printer<T> print_inline(matrix<T> const & m, char const * outer = ", ", char const * inner = ", ")
{
    return inline_matrix_printer<T>(m, outer, inner);
}

template <typename CharT, typename Traits, typename T>
std::basic_ostream<CharT, Traits> & operator<<(std::basic_ostream<CharT, Traits> & os,
                                               matrix_printer<T> const & mp)
{
    if (mp.mat.rows() == 0) { return os << mp.pre << "[]\n"; }

    for (std::size_t i = 0; i != mp.mat.rows(); ++i)
    {
        os << mp.pre << '|';
        for (std::size_t j = 0; j != mp.mat.cols(); ++j)
        {
            os << ' ' << std::setw(mp.wd) << std::right << mp.mat(i, j);
        }
        os << " |\n";
    }

    return os;
}

template <typename CharT, typename Traits, typename T>
std::basic_ostream<CharT, Traits> & operator<<(std::basic_ostream<CharT, Traits> & os,
                                               inline_matrix_printer<T> const & mp)
{
    os << '[';
    for (std::size_t i = 0; i != mp.mat.rows(); ++i)
    {
        if (i != 0) { os << mp.outer_sep; }
        os << '[';
        for (std::size_t j = 0; j != mp.mat.cols(); ++j)
        {
            if (j != 0) { os << mp.inner_sep; }
            os << mp.mat(i, j);
        }
        os << ']';
    }
    return os << ']';
}

#endif
