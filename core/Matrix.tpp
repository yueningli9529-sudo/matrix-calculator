#ifndef MATRIX_TPP_INCLUDED
#define MATRIX_TPP_INCLUDED

#define MATRIX_SKIP_IMPLEMENTATION_INCLUDES
#include "Matrix.hpp"
#undef MATRIX_SKIP_IMPLEMENTATION_INCLUDES


#include <cassert>


// ----------------constructor----------------
template <typename T>
Matrix<T>::Matrix(std::size_t row, std::size_t column, const T &value)
    : row_(row), column_(column), data(row * column, value) {}

template <typename T>
std::size_t Matrix<T>::row() const
{
    return row_;
}
template <typename T>
std::size_t Matrix<T>::column() const
{
    return column_;
}
template <typename T>
const std::vector<T> &Matrix<T>::Data() const
{
    return data;
}

// ——————————————access members——————————————————
template <typename T>
T &Matrix<T>::at(std::size_t i, std::size_t j)
{
    assert(i < row_ && j < column_);
    return data[i * column_ + j];
}
template <typename T>
const T &Matrix<T>::at(std::size_t i, std::size_t j) const
{
    assert(i < row_ && j < column_);
    return data[i * column_ + j];
}
template <typename T>
T &Matrix<T>::operator()(std::size_t i, std::size_t j)
{
    assert(i < row_ && j < column_);
    return data[i * column_ + j];
}
template <typename T>
const T &Matrix<T>::operator()(std::size_t i, std::size_t j) const
{
    assert(i < row_ && j < column_);
    return data[i * column_ + j];
}
// ——————————————————operator overload——————————————————————————————————
template <typename T>
Matrix<T> &Matrix<T>::operator+=(const Matrix<T> &b)
{
    assert(row_ == b.row_ && column_ == b.column_);
    for (std::size_t i = 0; i < row_ * column_; ++i)
    {
        data[i] += b.data[i];
    }
    return *this;
}
template <typename T>
Matrix<T> &Matrix<T>::operator-=(const Matrix<T> &b)
{
    assert(row_ == b.row_ && column_ == b.column_);
    for (std::size_t i = 0; i < row_ * column_; ++i)
    {
        data[i] -= b.data[i];
    }
    return *this;
}

// ————————————————————identity————————————————————————————————
template <typename T>
Matrix<T> Matrix<T>::identity(std::size_t n)
{
    Matrix<T> result(n, n, T{});
    for (std::size_t i = 0; i < n; i++)
    {
        result(i, i) = T{1};
    }
    return result;
}

// power
template <typename T>
Matrix<T> Matrix<T>::power(int a) const
{
    assert(row_ == column_ && a >= 0);
    Matrix<T> temp = identity(row_);
    for (; a > 0; a--)
    {
        temp = (*this) * temp;
    }
    return temp;
}

// ————————————————————transpose——————————————————————————
template <typename T>
Matrix<T> Matrix<T>::transpose() const
{
    Matrix<T> temp{column_, row_};
    for (std::size_t i = 0; i < row_; i++)
    {
        for (std::size_t j = 0; j < column_; j++)
        {
            temp(j, i) = (*this)(i, j);
        }
    }
    return temp;
}

// ————————————————————————ERO——————————————————————————————
template <typename T>
Matrix<T> &Matrix<T>::swap_two_lines(std::size_t s, std::size_t t, T *det)
{
    assert(s < row_ && t < row_);

    for (std::size_t i = 0; i < column_; ++i)
    {
        T temp = (*this)(s, i);
        (*this)(s, i) = (*this)(t, i);
        (*this)(t, i) = temp;
    }
    if (det)
    {
        *det *= T{-1};
    }

    return *this;
}
template <typename T>
Matrix<T> &Matrix<T>::multiply_one_line(const T &a, std::size_t s, T *det)
{
    for (std::size_t i = 0; i < column_; i++)
    {
        (*this)(s, i) = a * (*this)(s, i);
    }
    if (det)
    {
        *det *= T{1} / a;
    }
    return *this;
}
template <typename T>
Matrix<T> &Matrix<T>::add_one_to_another(const T &a, std::size_t s, std::size_t t, T *det)
{
    for (std::size_t i = 0; i < column_; ++i)
    {
        T temp = (*this)(s, i) * a;
        (*this)(t, i) += temp;
    }

    return *this;
}

// ——————————————REF————————————————————————
template <typename T>
Matrix<T> &Matrix<T>::find_std_leading(std::size_t &s_r, std::size_t &s_c, T *det)
{
    while (s_c < column_)
    {
        for (std::size_t i = s_r; i < row_; i++)
        {
            if ((*this)(i, s_c) != T{})
            {
                if (i != s_r)
                    swap_two_lines(s_r, i, det);
                return *this;
            }
        }
        s_c++;
    }
    return *this;
}
template <typename T>
Matrix<T> &Matrix<T>::reduce_by_std_leading(std::size_t &s_r, std::size_t &s_c, T *det)
{
    if (s_c >= column_)
    {
        return *this;
    }
    for (std::size_t i = s_r + 1; i < row_; i++)
    {
        T a = -(*this)(i, s_c) / (*this)(s_r, s_c);
        add_one_to_another(a, s_r, i, det);
    }
    return *this;
}
template <typename T>
Matrix<T> &Matrix<T>::change_leading_to_one(std::size_t &s_r, std::size_t &s_c, T *det)
{
    if (s_c >= column_)
    {
        return *this;
    }
    T a = T{1} / (*this)(s_r, s_c);
    multiply_one_line(a, s_r, det);
    return *this;
}
template <typename T>
Matrix<T> &Matrix<T>::one_REF(std::size_t &s_r, std::size_t &s_c, T *det)
{
    find_std_leading(s_r, s_c, det);
    change_leading_to_one(s_r, s_c, det);
    reduce_by_std_leading(s_r, s_c, det);
    s_r++;
    s_c++;
    return *this;
}
template <typename T>
Matrix<T> &Matrix<T>::REF(T *det)
{
    std::size_t s_r = 0;
    std::size_t s_c = 0;
    while (s_r < row_ && s_c < column_)
    {
        one_REF(s_r, s_c, det);
    }
    return *this;
}

// ——————————————-RREF——————————————————————————
template <typename T>
std::size_t Matrix<T>::find_the_leading_one_in_line_s(std::size_t s) const
{
    for (std::size_t j = 0; j < column_; j++)
    {
        if ((*this)(s, j) == T{1})
        {
            return j;
        }
    }
    return column_;
}
template <typename T>
Matrix<T> &Matrix<T>::change_column_to_zero(std::size_t row_lead, std::size_t col_lead)
{
    if (col_lead >= column_)
    {
        return *this;
    }
    for (std::size_t i = row_lead; i-- > 0;)
    {
        T temp = -(*this)(i, col_lead);
        add_one_to_another(temp, row_lead, i);
    }
    return *this;
}
template <typename T>
Matrix<T> &Matrix<T>::RREF()
{
    REF();
    for (std::size_t i = row_; i-- > 0;)
    {
        std::size_t s = find_the_leading_one_in_line_s(i);
        if (s < column_)
        {
            change_column_to_zero(i, s);
        }
    }
    return *this;
}

// ——————————————————————rank————————————————————————————————————
template <typename T>
std::size_t Matrix<T>::rank() const
{

    Matrix<T> tmp = *this;
    tmp.REF();
    std::size_t sum = 0;
    for (std::size_t i = 0; i < tmp.row(); i++)
    {
        std::size_t s = tmp.find_the_leading_one_in_line_s(i);
        if (s != tmp.column())
        {
            sum++;
        }
    }
    return sum;
}

// ————————————————————null & col——————————————————————————————
template <typename T>
Matrix<T> Matrix<T>::null() const
{
    Matrix<T> temp = (*this);
    temp.RREF();
    if (temp.rank() == temp.column_)
    {
        return Matrix<T>{temp.column_, 0, T{}};
    }
    std::size_t a = 0;
    Matrix<T> null{temp.column_, temp.column_ - temp.rank(), T{}};
    for (std::size_t j = 0; j < temp.column_; j++)
    {
        if (temp.is_variable(j))
        {
            std::size_t k = 0;
            for (std::size_t i = 0; i < j; i++)
            {
                if (!temp.is_variable(i))
                {
                    null(i, a) = -temp(k, j);
                    k++;
                }
            }
            null(j, a) = T{1};
            a++;
        }
    }
    return null;
}
template <typename T>
bool Matrix<T>::is_variable(std::size_t s) const
{
    for (std::size_t i = 0; i < row_; i++)
    {
        if (find_the_leading_one_in_line_s(i) == s)
        {
            return false;
        }
    }
    return true;
}
template <typename T>
Matrix<T> Matrix<T>::col() const
{
    Matrix<T> temp = (*this);
    temp.RREF();
    Matrix<T> col{row_, temp.rank(), T{}};
    std::size_t a = 0;
    for (std::size_t j = 0; j < temp.column_; j++)
    {
        if (!temp.is_variable(j))
        {
            for (std::size_t i = 0; i < row_; i++)
            {
                col(i, a) = (*this)(i, j);
            }
            a++;
        }
    }
    return col;
}

// ——————————————solve solution————————————————————
template <typename T>
Matrix<T> Matrix<T>::augmented(Matrix<T> const &b) const
{
    assert(row_ == b.row_ && b.column_ == 1);
    Matrix<T> temp{row_, column_ + 1, T{}};
    for (std::size_t i = 0; i < row_; i++)
    {
        for (std::size_t j = 0; j < column_ + 1; j++)
        {
            if (j != column_)
            {
                temp(i, j) = (*this)(i, j);
            }
            else
            {
                temp(i, j) = b(i, 0);
            }
        }
    }
    return temp;
}
template <typename T>
Matrix<T> Matrix<T>::solve(const Matrix<T> &b) const
{
    assert(row_ == b.row_ && b.column_ == 1);
    Matrix<T> temp = augmented(b);
    temp.RREF();
    Matrix<T> Aref = *this;
    Aref.RREF();
    if (Aref.rank() != temp.rank())
    {
        return Matrix<T>{0, 0, T{}};
    }
    Matrix<T> answer{column_, 1, T{}};
    std::size_t k = 0;
    for (std::size_t j = 0; j < column_; j++)
    {
        if (!temp.is_variable(j))
        {
            answer(j, 0) = temp(k, temp.column_ - 1);
            k++;
        }
    }
    return answer;
}
// ——————————————————det—————————————————————————————
template <typename T>
bool Matrix<T>::is_square() const
{
    return column_ == row_;
}
template <typename T>
T Matrix<T>::det() const
{
    assert(is_square());
    T det = T{1};
    Matrix<T> temp = (*this);
    temp.REF(&det);

    if (temp.rank() != row_)
        return T{};
    return det;
}

// reverse
template <typename T>
Matrix<T> Matrix<T>::inverse() const
{
    Matrix<T> temp2 = (*this);
    assert(is_square() && temp2.rank() == row_);
    Matrix<T> temp{row_, 2 * column_, T{}};
    for (std::size_t i = 0; i < temp.row_; i++)
    {
        for (std::size_t j = 0; j < temp.column_; j++)
        {
            if (j < column_)
            {
                temp(i, j) = (*this)(i, j);
            }
            else if (i + column_ == j)
            {
                temp(i, j) = T{1};
            }
        }
    }
    temp.RREF();
    Matrix<T> inverse{row_, column_, T{}};
    for (std::size_t i = 0; i < row_; i++)
    {
        for (std::size_t j = 0; j < column_; j++)
        {
            inverse(i, j) = temp(i, j + column_);
        }
    }
    return inverse;
}

// 每一个都要声明template <typename T>.  然后Matrix<T>::函数.....
// 初始化在声明的时候做，定义的时候就不用了
// 0->T{}

#endif // MATRIX_TPP_INCLUDED
