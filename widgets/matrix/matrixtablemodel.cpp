#include "matrixtablemodel.h"
#include <type_traits>

MatrixTableModel::MatrixTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int MatrixTableModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid())
    {
        return 0;
    }
    auto visitor = [](const auto &matrix)
    {
        std::size_t row_s = matrix.row();
        int row = static_cast<int>(row_s);
        return row;
    };
    int result = std::visit(visitor, this->matrix_);
    return result;
}

int MatrixTableModel::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid())
    {
        return 0;
    }
    auto visitor = [](const auto &matrix)
    {
        std::size_t column_s = matrix.column();
        int column = static_cast<int>(column_s);
        return column;
    };
    int result = std::visit(visitor, this->matrix_);
    return result;
}

QVariant MatrixTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
    {
        return {};
    }
    if(role != MatrixRole::DenominatorRole && role != MatrixRole::NumeratorRole  && role != Qt::DisplayRole && role != Qt::TextAlignmentRole)
    {
        return {};
    }

    int row = index.row();
    int column = index.column();

    if(role == MatrixRole::NumeratorRole)
    {
        auto visitor = [row,column](const auto &matrix)
        {
            using T = std::decay_t<decltype(matrix)>;
            if constexpr (std::is_same_v<T,Matrix<Rational>>)
            {
                QVariant numer = matrix(row,column).numer();
                return numer;
            }
            else
            {
                return QVariant{};
            }
        };
        return std::visit(visitor, this->matrix_);

    }
    else if(role == Qt::TextAlignmentRole)
    {
        return static_cast<int>(Qt::AlignCenter);
    }
    else if(role == MatrixRole::DenominatorRole)
    {
        auto visitor = [row,column](const auto &matrix)
        {
            using T = std::decay_t<decltype(matrix)>;
            if constexpr (std::is_same_v<T,Matrix<Rational>>)
            {
                QVariant denom = matrix(row,column).denom();
                return denom;
            }
            else
            {
                return QVariant{};
            }
        };
        return std::visit(visitor, this->matrix_);
    }
    else if(role == Qt::DisplayRole)
    {
        auto visitor = [row,column](const auto &matrix)
        {
            using T = std::decay_t<decltype(matrix)>;
            if constexpr (std::is_same_v<T,Matrix<Rational>>)
            {
                return QVariant{};
            }
            else
            {
                QVariant data = matrix(row,column);
                return data;
            }
        };
        return std::visit(visitor, this->matrix_);
    }
    return {};
}

void MatrixTableModel::setMatrix(const MatrixVariant &matrix)
{
    beginResetModel();
    matrix_ = matrix;
    endResetModel();
}
