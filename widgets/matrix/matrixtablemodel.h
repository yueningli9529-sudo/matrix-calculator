#ifndef MATRIXTABLEMODEL_H
#define MATRIXTABLEMODEL_H

#include <QAbstractTableModel>
#include "MatrixType.h"

class MatrixTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum MatrixRole
    {
        NumeratorRole = Qt::UserRole + 1,
        DenominatorRole
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    void setMatrix(const MatrixVariant &matrix);
    explicit MatrixTableModel(QObject *parent = nullptr);
private:
    MatrixVariant matrix_;
};

#endif // MATRIXTABLEMODEL_H
