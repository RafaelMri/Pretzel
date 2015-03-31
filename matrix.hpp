// Matrix utilities:
//
//    - A matrix class template: matrix<T>
//    - A special square matrix version: square_matrix<T>
//    - vandermonde() generates a Vandermonde square matrix
//
// Both versions support Gauss and Gauss-Jordan elimination.
// The square version also supports determinant computation.
// Both versions support writing a string representation to
// an std::basic_ostream.

#ifndef H_MATRIX
#define H_MATRIX

#include <algorithm>
#include <cassert>
#include <cmath>
#include <initializer_list>
#include <iosfwd>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <vector>

template <typename> class square_matrix;

template <typename T>
class matrix
{
    template <typename> friend class matrix;
    friend class square_matrix<T>;

public:
    explicit matrix(std::size_t rows, std::size_t cols, T const & val)
    : rows_(rows)
    , cols_(cols)
    , data_(rows_ * cols_, val)
    { }

    explicit matrix(std::size_t rows, std::size_t cols)
    : matrix(rows, cols, T())
    { }

private:
    // Internal constructor from raw storage
    explicit matrix(std::size_t rows, std::size_t cols, std::vector<T> data)
    : rows_(rows)
    , cols_(cols)
    , data_(std::move(data))
    { }

public:
    template <typename S> operator matrix<S>() const
    {
        return matrix<S>(rows_, cols_, std::vector<S>(data_.begin(), data_.end()));
    }

    std::size_t rows() const { return rows_; }
    std::size_t cols() const { return cols_; }

    T const & operator()(std::size_t i, std::size_t j) const { return data_[i * cols_ + j]; }
    T       & operator()(std::size_t i, std::size_t j)       { return data_[i * cols_ + j]; }

    matrix transpose() const
    {
        std::vector<T> new_data;
        new_data.reserve(data_.size());
        for (std::size_t j = 0; j != cols_; ++j)
            for (std::size_t i = 0; i != rows_; ++i)
                new_data.push_back(data_[i * cols_ + j]);
        return matrix(cols_, rows_, std::move(new_data));
    }

    // Various elimination forms. See below for details.
    matrix gauss(bool unit_diagonal, int * swap_count) const;
    matrix gauss_jordan() const;

    // Swapping rows or columns is done on the matrix itself.
    void swap_rows(std::size_t r1, std::size_t r2)
    {
        assert(r1 < rows_ && r2 < rows_);

        for (std::size_t j = 0; j != cols_; ++j)
        {
            using std::swap;
            swap((*this)(r1, j), (*this)(r2, j));
        }
    }
    void swap_cols(std::size_t c1, std::size_t c2)
    {
        assert(c1 < cols_ && c2 < cols_);

        for (std::size_t i = 0; i != rows_; ++i)
        {
            using std::swap;
            swap((*this)(i, c1), (*this)(i, c2));
        }
    }

    // Removing rows and columns is done on the matrix itself.
    void remove_row(std::size_t r)
    {
        assert(r < rows_);

        data_.erase(data_.begin() + r * cols_, data_.begin() + (r + 1) * cols_);
        --rows_;
        if (rows_ == 0) { cols_ = 0; assert(data_.empty()); }
    }
    void remove_col(std::size_t c)
    {
        assert(c < cols_);

        auto it = data_.begin() + c;
        for (std::size_t i = 0; i != rows_ - 1; ++i)
        {
            it = data_.erase(it) + (cols_ - 1);
        }
        data_.erase(it);
        --cols_;
        if (cols_ == 0) { rows_ = 0; assert(data_.empty()); }
    }

    matrix<T> operator+(const matrix<T> & rhs) const
    {
        if (rows() != rhs.rows() || cols() != rhs.cols())
            throw std::domain_error("Trying to add matrices of different sizes!");

        std::vector<T> new_data = data_;
        for (std::size_t i = 0; i != new_data.size(); ++i) { new_data[i] += rhs.data_[i]; }
        return matrix(rows_, cols_, std::move(new_data));
    }

    matrix<T> operator*(const T & x) const
    {
        std::vector<T> new_data = data_;
        for (T & t : new_data) { t *= x; }
        return matrix(rows_, cols_, std::move(new_data));
    }

protected:
    std::size_t rows_;
    std::size_t cols_;
    std::vector<T> data_;
};

// Gaussian elimination: returns a row-echelon form; if unit_diagonal is true, then
// with "1" along the diagonal. If swap_count is non-null, the pointed-to integer will
// be incremented for every row swap (which may be needed to determine the sign of the
// determinant). Requires division.
template <typename T>
matrix<T> matrix<T>::gauss(bool unit_diagonal, int * swap_count) const
{
    static_assert(std::is_floating_point<T>::value,
                  "Gauss elimination can only be performed on a divisible number type.");

    matrix<T> m(*this);

    for (std::size_t i = 0, j = 0; i < m.rows() && j < m.cols(); ++j /* only j! */)
    {
        // Find pivotable row.
        std::size_t max_i = i;
        for (std::size_t k = i + 1; k < m.rows(); ++k)
            if (std::abs(m(k, j)) > std::abs(m(max_i, j)))
                max_i = k;

        // No action needed if the largest element is already zero.
        if (m(max_i, j) == 0) { continue; }

        // Swap with pivot row.
        if (i != max_i)
        {
            m.swap_rows(i, max_i);
            if (swap_count) { ++*swap_count; }
        }

        // Divide each entry in row i by m(i, j);
        // scale the diagonal to unity if requested.
        T divisor = m(i, j);
        if (unit_diagonal)
        {
            m(i, j) = T(1);
            for (std::size_t l = j + 1; l < cols();  ++l)
                m(i, l) /= divisor;

            for (size_t k = i + 1; k < rows(); ++k)
            {
                T tmp = m(k, j);
                for (size_t l = 0; l < cols();  ++l)
                    m(k, l) -= (tmp * m(i, l));
            }
        }
        else
        {
            for (std::size_t k = i + 1; k < rows(); ++k)
            {
                T tmp = m(k, j);
                for (std::size_t l = 0; l < cols();  ++l)
                    m(k, l) -= (tmp / divisor * m(i, l));
                }
        }

        // Only advance i if we got here.
        ++i;
    }

    return m;
}

// Gauss-Jordan elimination: returns the reduced row-echolon form. Computed via
// backsubstition in the result of gauss(). Requires division.
template <typename T>
matrix<T> matrix<T>::gauss_jordan() const
{
    matrix<T> m(this->gauss(true, nullptr));

    if (rows() == 0) { return m; }

    // subtract m(i,j) * row(j) from row(i), going backwards
    for (std::size_t i = 0; i + 1 < rows(); ++i)
    {
        for (std::size_t j = 0; j < i + 1; ++j)
        {
            std::size_t const ri = rows() - i - 2;
            std::size_t const rj = rows() - j - 1;

            T tmp = m(ri, rj);
            for (std::size_t l = 0; l != cols(); ++l)
                m(ri, l) -= tmp * m(rj, l);
        }
    }

    return m;
}

template <typename T>
class square_matrix : public matrix<T>
{
public:
    explicit square_matrix(std::size_t dim)
    : square_matrix(dim, T())
    { }

    explicit square_matrix(std:: size_t dim, T const & val)
    : matrix<T>(dim, dim, val)
    { }

    square_matrix(matrix<T> const & m)
    : matrix<T>(m)
    {
        if (m.rows() != m.cols())
            throw std::domain_error("Trying to construct square matrix from non-square matrix!");
    }

    square_matrix(matrix<T> && m)
    : matrix<T>(std::move(m))
    {
        if (m.rows() != m.cols())
            throw std::domain_error("Trying to construct square matrix from non-square matrix!");
    }

    std::size_t dim() const { return this->rows(); }

    square_matrix transpose() const
    {
        using std::swap;
        square_matrix result(*this);
        for (std::size_t i = 0; i != dim(); ++i)
            for (std::size_t j = i + 1; j < dim(); ++j)
                swap(result(i, j), result(j, i));
        return result;
    }

    T determinant() const
    {
        // Gauss elimination followed by multiplying up the diagonal.
        int swapcount = 0;
        square_matrix gaussed = this->gauss(false, &swapcount);
        T det(1);
        for (std::size_t i = 0; i != dim(); ++i) { det *= gaussed(i, i); }
        return (swapcount % 2  ?  -det  :  det);
    }

    square_matrix operator+(square_matrix const & x) const
    {
        return square_matrix(static_cast<matrix<T> const *>(this)->operator+(x));
    }

    square_matrix operator*(T const & x) const
    {
        return square_matrix(static_cast<matrix<T> const *>(this)->operator*(x));
    }
};

template <typename T, typename C>
matrix<T> vandermonde(std::size_t n, C const & data)
{
    using std::begin;
    using std::end;

    std::size_t m = std::distance(begin(data), end(data));

    matrix<T> result(m, n);

    auto it = begin(data);
    for (std::size_t i = 0; i != m; ++i)
    {
        for (std::size_t j = 0; j != n; ++j)
        {
            result(i, j) = std::pow(*it, j);
        }
        ++it;
    }

    return result;
}

template <typename T>
matrix<T> vandermonde(std::size_t n, std::initializer_list<T> il)
{
    return vandermonde<T, std::initializer_list<T>>(n, il);
}

#endif
