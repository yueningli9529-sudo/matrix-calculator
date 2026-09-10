#ifndef MATRIXSTEP_H
#define MATRIXSTEP_H
#include <variant>
#include <cstddef>
#include "Rational.hpp"
#include "MatrixType.h"

using ScalarVariant = std::variant<Rational, double, int>;

struct SwapRowsOperation
{
    std::size_t firstRowIndex_;
    std::size_t secondRowIndex_;
};

struct AddScaledRowOperation
{
    std::size_t targetRowIndex_;
    ScalarVariant factor_;
    std::size_t sourceRowIndex_;
};

struct ScaleRowOperation
{
    ScalarVariant factor_;
    std::size_t rowIndex_;
};
using RowOperation = std::variant<SwapRowsOperation, AddScaledRowOperation, ScaleRowOperation>;

struct MatrixStep
{
    RowOperation operation_;
    MatrixVariant resultMatrix_;
};

#endif // MATRIXSTEP_H
