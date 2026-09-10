#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "pages/inputpage.h"
#include "pages/calculatepage.h"
#include "MatrixType.h"
#include "MatrixSaveSystem.h"
#include "dialog.h"
#include <QGridLayout>
#include <QSpinBox>
#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow *ui;
    InputPage *IPage;
    CalculatePage *CPage;
    MatrixRepository repository_;
};
#endif // MAINWINDOW_H
