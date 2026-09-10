#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <cstddef>
#include <vector>
#include <iosfwd>
#include <stdexcept>

template <typename T>
class Matrix
{
public:
    // constructor
    Matrix(std::size_t row = 0, std::size_t column = 0, const T &value = T{});
    std::size_t row() const;
    std::size_t column() const;
    const std::vector<T> &Data() const;
    // access members
    T &at(std::size_t i, std::size_t j);
    const T &at(std::size_t i, std::size_t j) const;
    T &operator()(std::size_t i, std::size_t j);
    const T &operator()(std::size_t i, std::size_t j) const;
    // operator overload
    Matrix &operator+=(const Matrix &b);
    Matrix &operator-=(const Matrix &b);
    // identity
    static Matrix identity(std::size_t n);
    // power
    Matrix power(int a) const;
    // transpose
    Matrix transpose() const;
    // ERO
    Matrix &swap_two_lines(std::size_t s, std::size_t t, T *det = nullptr);
    Matrix &multiply_one_line(const T &a, std::size_t s, T *det = nullptr);
    Matrix &add_one_to_another(const T &a, std::size_t s, std::size_t t, T *det = nullptr);
    // REF
    Matrix &find_std_leading(std::size_t &s_r, std::size_t &s_c, T *det = nullptr);
    Matrix &reduce_by_std_leading(std::size_t &s_r, std::size_t &s_c, T *det = nullptr);
    Matrix &change_leading_to_one(std::size_t &s_r, std::size_t &s_c, T *det = nullptr);
    Matrix &one_REF(std::size_t &s_r, std::size_t &s_c, T *det = nullptr);
    Matrix &REF(T *det = nullptr);
    // RREF
    std::size_t find_the_leading_one_in_line_s(std::size_t s) const;
    Matrix &change_column_to_zero(std::size_t row_lead, std::size_t col_lead);
    Matrix &RREF();
    // rank
    std::size_t rank() const;
    // null & col
    Matrix null() const;
    bool is_variable(std::size_t s) const;
    Matrix col() const;
    // solve equation
    Matrix augmented(Matrix const &b) const;
    Matrix solve(const Matrix &b) const;
    // det
    bool is_square() const;
    T det() const;
    // reverse
    Matrix inverse() const;

private:
    std::size_t row_{0};
    std::size_t column_{0};
    std::vector<T> data;
};

// operation overload
template <typename U>
std::ostream &operator<<(std::ostream &out, const Matrix<U> &a);

template <typename U>
Matrix<U> operator+(const Matrix<U> &a, const Matrix<U> &b);

template <typename U>
Matrix<U> operator-(const Matrix<U> &a);

template <typename U>
Matrix<U> operator-(const Matrix<U> &a, const Matrix<U> &b);

template <typename U>
Matrix<U> operator*(const U &s, const Matrix<U> &a);

template <typename U>
Matrix<U> operator*(const Matrix<U> &a, const U &s);

template <typename U>
Matrix<U> operator*(const Matrix<U> &a, const Matrix<U> &b);

template <typename U>
std::istream &operator>>(std::istream &in, Matrix<U> &a);

#ifndef MATRIX_SKIP_IMPLEMENTATION_INCLUDES
#include "Matrix.tpp"
#include "MatrixOps.tpp"
#endif

#endif

// T value =T{} 模版不能保证T能从0转换
// 自己不变的就一定要加上const
// at（）要特别注意返回的是应用，一个读一个改，前const是元素不变，后const是矩阵不变
