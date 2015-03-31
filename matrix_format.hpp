// Matrix pretty-printing tools.
//
// 1. Inline printer:
//
//    matrix<int> m = make_a_matrix();
//    std::cout << "The matrix is " << print_inline(m) << ".\n";
//
// 2. Full multi-line pretty-print with fixed field width:
//
//    matrix<int> m = make_a_matrix();
//    std::cout << "The matrix is:\n" << print(m, 4);

#ifndef H_MATRIX_FORMAT
#define H_MATRIX_FORMAT

#include <iomanip>
#include <iosfwd>

#include "matrix.hpp"

template <typename T>
struct matrix_printer
{
    matrix<T> const & mat;     // the matrix
    char const * pre;          // per-line prefix (e.g. indentation)
    int wd;                    // field width

    matrix_printer(matrix<T> const & m, char const * p, int w = 0)
    : mat(m), pre(p), wd(w) {}
};

template <typename T>
struct inline_matrix_printer
{
    matrix<T> const & mat;     // the matrix
    char const * outer_sep;    // separator between rows
    char const * inner_sep;    // separator between columns

    inline_matrix_printer(matrix<T> const & m, char const * outer, char const * inner)
    : mat(m), outer_sep(outer), inner_sep(inner) {}
};

// Returns a multi-line matrix pretty printer with the given field width
// and per-line prefix.
template <typename T>
matrix_printer<T> print(matrix<T> const & m, int w, char const * p = "")
{
    return matrix_printer<T>(m, p, w);
}

// Returns an inline matrix pretty printer with the given row and column
// separators.
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
