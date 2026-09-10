#ifndef MATRIXWIDGET_H
#define MATRIXWIDGET_H

#include <QWidget>
#include "MatrixType.h"

class MatrixTableModel;
namespace Ui {
class MatrixWidget;
}

class MatrixWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MatrixWidget(QWidget *parent = nullptr);
    void setMatrix(const MatrixVariant &matrix);
    ~MatrixWidget();

private:
    Ui::MatrixWidget *ui;
    MatrixTableModel *model_;
};

#endif // MATRIXWIDGET_H
