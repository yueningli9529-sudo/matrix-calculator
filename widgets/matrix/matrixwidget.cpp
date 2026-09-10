#include "matrixwidget.h"
#include "ui_matrixwidget.h"
#include "matrixtablemodel.h"
#include "matrixitemdelegate.h"
#include <QSizePolicy>
#include <QAbstractItemView>

MatrixWidget::MatrixWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MatrixWidget)
    , model_(new MatrixTableModel(this))
{
    ui->setupUi(this);
    ui->matrixTableView->setModel(model_);
    MatrixItemDelegate *delegate = new MatrixItemDelegate(ui -> matrixTableView);
    ui->matrixTableView->setItemDelegate(delegate);

    //disable scroll bars
    ui->matrixTableView->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff);

    ui->matrixTableView->setVerticalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    //brackets
    ui->leftBracketWidget->setStyleSheet(
        "#leftBracketWidget {"
        "border-left: 2px solid palette(text);"
        "border-top: 2px solid palette(text);"
        "border-bottom: 2px solid palette(text);"
        "}");

    ui->rightBracketWidget->setStyleSheet(
        "#rightBracketWidget {"
        "border-right: 2px solid palette(text);"
        "border-top: 2px solid palette(text);"
        "border-bottom: 2px solid palette(text);"
        "}");
    //set transparent
    ui->matrixTableView->setStyleSheet(
        "QTableView {"
        "background: transparent;"
        "border: none;"
        "}");
    ui->matrixTableView->viewport()->setAutoFillBackground(false);
    ui->matrixTableView->viewport()->setAttribute(
        Qt::WA_TranslucentBackground);
    //set unclickable
    ui->matrixTableView->setSelectionMode(
        QAbstractItemView::NoSelection);
    ui->matrixTableView->setEditTriggers(
        QAbstractItemView::NoEditTriggers);
    ui->matrixTableView->setFocusPolicy(Qt::NoFocus);
    hide();
}

void MatrixWidget::setMatrix(const MatrixVariant &matrix)
{
    model_->setMatrix(matrix);
    const int rows = model_->rowCount();
    const int columns = model_->columnCount();

    if (rows <= 0 || columns <= 0)
    {
        hide();
        return;
    }
    //read sizehint
    ui->matrixTableView->resizeRowsToContents();
    ui->matrixTableView->resizeColumnsToContents();

    //min size
    int cellSize = 40;
    //max size
    for (int column = 0; column < columns; ++column)
    {
        const int currentWidth =
            ui->matrixTableView->columnWidth(column);

        cellSize = qMax(cellSize, currentWidth);
    }
    for (int row = 0; row < rows; ++row)
    {
        const int currentHeight =
            ui->matrixTableView->rowHeight(row);

        cellSize = qMax(cellSize, currentHeight);
    }
    //use size in every row & column
    for (int column = 0; column < columns; ++column)
    {
        ui->matrixTableView->setColumnWidth(column, cellSize);
    }

    for (int row = 0; row < rows; ++row)
    {
        ui->matrixTableView->setRowHeight(row, cellSize);
    }
    //set view size
    const int tableWidth =
        columns * cellSize +
        ui->matrixTableView->frameWidth() * 2;

    const int tableHeight =
        rows * cellSize +
        ui->matrixTableView->frameWidth() * 2;

    ui->matrixTableView->setFixedSize(tableWidth, tableHeight);
    show();
    adjustSize();
    updateGeometry();
}

MatrixWidget::~MatrixWidget()
{
    delete ui;
}
