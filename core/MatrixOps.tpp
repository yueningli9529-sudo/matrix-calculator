#ifndef MATRIX_OPS_TPP_INCLUDED
#define MATRIX_OPS_TPP_INCLUDED

#define MATRIX_SKIP_IMPLEMENTATION_INCLUDES
#include "Matrix.hpp"
#undef MATRIX_SKIP_IMPLEMENTATION_INCLUDES

#include <ostream>
#include <istream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cassert>

// —————————————————————operation overload——————————————————
template <typename T>
std::ostream &operator<<(std::ostream &out, const Matrix<T> &a)
{
    std::vector<std::size_t> col_width(a.column(), 0);

    // 计算每列最大宽度
    for (std::size_t j = 0; j < a.column(); j++)
    {
        for (std::size_t i = 0; i < a.row(); i++)
        {
            std::ostringstream oss;
            oss << a(i, j);
            col_width[j] = std::max(col_width[j], oss.str().length());
        }
    }

    // 输出矩阵，每列固定宽度，左对齐
    for (std::size_t i = 0; i < a.row(); i++)
    {
        for (std::size_t j = 0; j < a.column(); j++)
        {
            std::ostringstream oss;
            oss << a(i, j);
            out << std::left << std::setw(col_width[j]) << oss.str();
            if (j != a.column() - 1)
                out << ' '; // 列间空格
        }
        out << std::endl;
    }

    return out;
}
template <typename T>
Matrix<T> operator+(const Matrix<T> &a, const Matrix<T> &b)
{
    assert(a.column() == b.column() && a.row() == b.row());
    Matrix<T> c{a.row(), a.column(), T{}};
    for (std::size_t i = 0; i < a.row(); i++)
    {
        for (std::size_t j = 0; j < a.column(); j++)
        {
            c(i, j) = a(i, j) + b(i, j);
        }
    }
    return c;
}
template <typename T>
Matrix<T> operator-(const Matrix<T> &a)
{
    Matrix<T> c{a.row(), a.column(), T{}};
    for (std::size_t i = 0; i < a.row(); i++)
    {
        for (std::size_t j = 0; j < a.column(); j++)
        {
            c(i, j) = -a(i, j);
        }
    }
    return c;
}
template <typename T>
Matrix<T> operator-(const Matrix<T> &a, const Matrix<T> &b)
{
    return a + (-b);
}
template <typename U>
Matrix<U> operator*(const U &s, const Matrix<U> &a)
{
    Matrix<U> c{a.row(), a.column(), U{}};
    for (std::size_t i = 0; i < a.row(); i++)
    {
        for (std::size_t j = 0; j < a.column(); j++)
        {
            c(i, j) = s * a(i, j);
        }
    }
    return c;
}
template <typename U>
Matrix<U> operator*(const Matrix<U> &a, const U &s)
{
    return s * a;
}
template <typename T>
Matrix<T> operator*(const Matrix<T> &a, const Matrix<T> &b)
{
    assert(a.column() == b.row());
    Matrix<T> c{a.row(), b.column(), T{}};
    for (std::size_t i = 0; i < a.row(); i++)
    {
        for (std::size_t j = 0; j < b.column(); j++)
        {
            for (std::size_t k = 0; k < a.column(); k++)
            {
                c(i, j) += a(i, k) * b(k, j);
            }
        }
    }
    return c;
}

template <typename T>
std::istream &operator>>(std::istream &in, Matrix<T> &a)
{
    for (std::size_t i = 0; i < a.row(); ++i)
        for (std::size_t j = 0; j < a.column(); ++j)
            in >> a(i, j);
    return in;
}

#endif // MATRIX_OPS_TPP_INCLUDED
