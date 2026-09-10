#ifndef MATRIXITEMDELEGATE_H
#define MATRIXITEMDELEGATE_H

#include <QStyledItemDelegate>

class MatrixItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit MatrixItemDelegate(QObject *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // MATRIXITEMDELEGATE_H
