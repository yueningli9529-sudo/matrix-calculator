#pragma once

#include "Matrix.hpp"
#include "Rational.hpp"

#include <variant>

using MatrixVariant = std::variant<Matrix<Rational>, Matrix<double>, Matrix<int>>;
