// tests.cpp
#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>

// 改成你的头文件路径
#include "Matrix.hpp"
#include "Rational.hpp"

static int g_pass = 0;
static int g_fail = 0;

static void require(bool cond, const std::string &msg)
{
    if (!cond)
        throw std::runtime_error(msg);
}

static void test_input_operator_int()
{
    Matrix<int> A(2, 2, 0);
    std::stringstream ss("1 2 3 4");
    ss >> A;
    require(A(0, 0) == 1 && A(0, 1) == 2 && A(1, 0) == 3 && A(1, 1) == 4, "operator>> failed");
}

static void run_test(const std::string &name, void (*fn)())
{
    try
    {
        fn();
        ++g_pass;
        std::cout << "[PASS] " << name << "\n";
    }
    catch (const std::exception &e)
    {
        ++g_fail;
        std::cout << "[FAIL] " << name << " -> " << e.what() << "\n";
    }
    catch (...)
    {
        ++g_fail;
        std::cout << "[FAIL] " << name << " -> unknown error\n";
    }
}

// ---------------- helper: exact matrix equality (works for int/Rational) ----------------
template <typename T>
static bool mat_equal(const Matrix<T> &A, const Matrix<T> &B)
{
    if (A.row() != B.row() || A.column() != B.column())
        return false;
    for (std::size_t i = 0; i < A.row(); ++i)
        for (std::size_t j = 0; j < A.column(); ++j)
            if (A(i, j) != B(i, j))
                return false;
    return true;
}

template <typename T>
static Matrix<T> identity(std::size_t n)
{
    return Matrix<T>::identity(n);
}

// ---------------- tests ----------------
static void test_construct_and_access_int()
{
    Matrix<int> A(2, 3, 7);
    require(A.row() == 2 && A.column() == 3, "shape wrong");
    for (std::size_t i = 0; i < 2; ++i)
        for (std::size_t j = 0; j < 3; ++j)
            require(A(i, j) == 7, "init value wrong");
    A(1, 2) = 42;
    require(A(1, 2) == 42, "write/read failed");
}

static void test_add_sub_int()
{
    Matrix<int> A(2, 2, 0), B(2, 2, 0);
    A(0, 0) = 1;
    A(0, 1) = 2;
    A(1, 0) = 3;
    A(1, 1) = 4;
    B(0, 0) = 10;
    B(0, 1) = 20;
    B(1, 0) = 30;
    B(1, 1) = 40;

    Matrix<int> C = A + B;
    require(C(1, 1) == 44, "A+B wrong");

    Matrix<int> D = B - A;
    require(D(0, 0) == 9 && D(1, 1) == 36, "B-A wrong");

    Matrix<int> E = -A;
    require(E(1, 0) == -3, "-A wrong");
}

static void test_mul_int()
{
    // A 2x3, B 3x2
    Matrix<int> A(2, 3, 0), B(3, 2, 0);
    // A = [1 2 3; 4 5 6]
    int v = 1;
    for (std::size_t i = 0; i < 2; ++i)
        for (std::size_t j = 0; j < 3; ++j)
            A(i, j) = v++;
    // B = [7 8; 9 10; 11 12]
    v = 7;
    for (std::size_t i = 0; i < 3; ++i)
        for (std::size_t j = 0; j < 2; ++j)
            B(i, j) = v++;

    Matrix<int> C = A * B; // 2x2
    // 手算：C00=1*7+2*9+3*11=58; C01=1*8+2*10+3*12=64
    require(C.row() == 2 && C.column() == 2, "shape wrong");
    require(C(0, 0) == 58 && C(0, 1) == 64, "mul first row wrong");
    // C10=4*7+5*9+6*11=139; C11=4*8+5*10+6*12=154
    require(C(1, 0) == 139 && C(1, 1) == 154, "mul second row wrong");
}

static void test_transpose_int()
{
    Matrix<int> A(2, 3, 0);
    // [1 2 3; 4 5 6]
    int v = 1;
    for (std::size_t i = 0; i < 2; ++i)
        for (std::size_t j = 0; j < 3; ++j)
            A(i, j) = v++;

    Matrix<int> T = A.transpose(); // 3x2
    require(T.row() == 3 && T.column() == 2, "transpose shape wrong");
    require(T(0, 1) == 4 && T(2, 0) == 3, "transpose values wrong");
}

// det 用 Rational 测（你的 REF/RREF/DET 需要“能做分数除法”的类型）
static void test_det_rational()
{
    // det([1 2; 3 4]) = -2
    Matrix<Rational> A(2, 2, Rational{0});
    A(0, 0) = Rational{1};
    A(0, 1) = Rational{2};
    A(1, 0) = Rational{3};
    A(1, 1) = Rational{4};
    require(A.det() == Rational{-2}, "det nonzero wrong");

    // det([1 2; 2 4]) = 0
    Matrix<Rational> B(2, 2, Rational{0});
    B(0, 0) = Rational{1};
    B(0, 1) = Rational{2};
    B(1, 0) = Rational{2};
    B(1, 1) = Rational{4};
    require(B.det() == Rational{0}, "det zero wrong");
}

// 可选：inverse 用 Rational 测（强烈推荐加上）
static void test_inverse_rational()
{
    Matrix<Rational> A(2, 2, Rational{0});
    A(0, 0) = Rational{1};
    A(0, 1) = Rational{2};
    A(1, 0) = Rational{3};
    A(1, 1) = Rational{4};

    Matrix<Rational> inv = A.inverse();
    Matrix<Rational> I = A * inv;

    require(mat_equal(I, identity<Rational>(2)), "A*inv != I");
}
static void test_null_rational()
{
    // A = [1 2 3
    //      2 4 6]  rank = 1, null dim = 2
    Matrix<Rational> A(2, 3, Rational{0});
    A(0, 0) = Rational{1};
    A(0, 1) = Rational{2};
    A(0, 2) = Rational{3};
    A(1, 0) = Rational{2};
    A(1, 1) = Rational{4};
    A(1, 2) = Rational{6};

    Matrix<Rational> N = A.null();
    require(N.row() == 3, "null(): basis rows must be n (number of columns)");
    require(N.column() == 2, "null(): dimension should be n-rank (=2)");

    Matrix<Rational> Z = A * N; // 2 x 2
    require(Z.row() == 2 && Z.column() == 2, "A*null shape wrong");

    for (std::size_t i = 0; i < Z.row(); ++i)
        for (std::size_t j = 0; j < Z.column(); ++j)
            require(Z(i, j) == Rational{0}, "A*null != 0");
}
static void test_solve_unique_rational()
{
    // A = [2 1; 5 3], b = [1; 2]
    // 解：x = [1; -1]
    Matrix<Rational> A(2, 2, Rational{0});
    A(0, 0) = Rational{2};
    A(0, 1) = Rational{1};
    A(1, 0) = Rational{5};
    A(1, 1) = Rational{3};

    Matrix<Rational> b(2, 1, Rational{0});
    b(0, 0) = Rational{1};
    b(1, 0) = Rational{2};

    Matrix<Rational> x = A.solve(b);
    require(x.row() == 2 && x.column() == 1, "solve(): wrong shape for unique solution");

    Matrix<Rational> check = A * x;
    require(mat_equal(check, b), "solve(): A*x != b");
}
static void test_solve_no_solution_rational()
{
    // A = [1 1; 2 2], b = [1; 3]  -> 不一致，无解
    Matrix<Rational> A(2, 2, Rational{0});
    A(0, 0) = Rational{1};
    A(0, 1) = Rational{1};
    A(1, 0) = Rational{2};
    A(1, 1) = Rational{2};

    Matrix<Rational> b(2, 1, Rational{0});
    b(0, 0) = Rational{1};
    b(1, 0) = Rational{3};

    Matrix<Rational> x = A.solve(b);
    require(x.row() == 0 && x.column() == 0, "solve(): expected 0x0 for no solution");
}
static void test_solve_infinite_solutions_rational()
{
    // A = [1 1; 2 2], b = [3; 6]  -> 无穷多解
    Matrix<Rational> A(2, 2, Rational{0});
    A(0, 0) = Rational{1};
    A(0, 1) = Rational{1};
    A(1, 0) = Rational{2};
    A(1, 1) = Rational{2};

    Matrix<Rational> b(2, 1, Rational{0});
    b(0, 0) = Rational{3};
    b(1, 0) = Rational{6};

    Matrix<Rational> x = A.solve(b);
    require(x.row() == 2 && x.column() == 1, "solve(): wrong shape for infinite-solution case");

    Matrix<Rational> check = A * x;
    require(mat_equal(check, b), "solve(): returned x does not satisfy A*x=b");
}
static void test_col_rational()
{
    // 3 columns but col2 = col1 + col0, so rank=2
    // A = [1 0 1
    //      0 1 1]
    Matrix<Rational> A(2, 3, Rational{0});
    A(0, 0) = Rational{1};
    A(1, 0) = Rational{0};
    A(0, 1) = Rational{0};
    A(1, 1) = Rational{1};
    A(0, 2) = Rational{1};
    A(1, 2) = Rational{1};

    Matrix<Rational> C = A.col();
    require(C.row() == 2, "col(): wrong number of rows");
    require(C.column() == 2, "col(): should return rank columns (=2)");

    // 每一列都必须等于原矩阵的某一列
    for (std::size_t j = 0; j < C.column(); ++j)
    {
        bool found = false;
        for (std::size_t k = 0; k < A.column(); ++k)
        {
            bool same = true;
            for (std::size_t i = 0; i < A.row(); ++i)
                if (C(i, j) != A(i, k))
                {
                    same = false;
                    break;
                }

            if (same)
            {
                found = true;
                break;
            }
        }
        require(found, "col(): returned a column not from original matrix");
    }

    // 返回的列空间基应该满列秩（rank == number of columns）
    require(C.rank() == C.column(), "col(): returned columns are not independent");
}

int main()
{
    run_test("construct & access (int)", test_construct_and_access_int);
    run_test("add/sub/neg (int)", test_add_sub_int);
    run_test("mul (int)", test_mul_int);
    run_test("transpose (int)", test_transpose_int);
    run_test("det (Rational)", test_det_rational);
    run_test("inverse (Rational)", test_inverse_rational);
    run_test("operator>> (int)", test_input_operator_int);
    run_test("null (Rational)", test_null_rational);
    run_test("solve unique (Rational)", test_solve_unique_rational);
    run_test("solve none (Rational)", test_solve_no_solution_rational);
    run_test("solve infinite (Rational)", test_solve_infinite_solutions_rational);
    run_test("col (Rational)", test_col_rational);

    std::cout << "\nSummary: " << g_pass << " passed, " << g_fail << " failed.\n";
    return g_fail == 0 ? 0 : 1;
}

// g++ -std=c++11 tests.cpp Rational.cpp -o tests
//./tests