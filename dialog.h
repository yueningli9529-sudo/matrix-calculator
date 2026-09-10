#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "MatrixType.h"
#include <QGridLayout>
#include <QSpinBox>
#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    MatrixVariant matrix() const;
    ~Dialog();

private:
    Ui::Dialog *ui;
    MatrixVariant editing_matrix_;
    QGridLayout *matrixLayout_{nullptr};

    void clear_layout(QLayout *layout);
    void build_matrix();
    void load_input();
};

#endif // DIALOG_H
