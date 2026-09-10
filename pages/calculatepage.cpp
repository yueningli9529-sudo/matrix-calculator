#include "calculatepage.h"
#include "ui_calculatepage.h"
#include <variant>
#include <QMessageBox>
#include <type_traits>
#include <cmath>

CalculatePage::CalculatePage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CalculatePage)
{
    ui->setupUi(this);
    //set size
    ui->splitter->setStretchFactor(0,3);
    ui->splitter->setStretchFactor(1,1);
    ui->splitter->setSizes({750,250});
    setMode(CalculationMode::Single);

    //show matrix
    QObject::connect(
        ui->upperMatrixComboBox,
        &QComboBox::currentIndexChanged,
        this,
        [this](){
            showUpperMatrix();
        });
    QObject::connect(
        ui->lowerMatrixComboBox,
        &QComboBox::currentIndexChanged,
        this,
        [this](){
            showLowerMatrix();
        });
    //set mode
    QObject::connect(
        ui->singleModeButton,
        &QToolButton::clicked,
        this,
        [this](){
            setMode(CalculationMode::Single);
        });
    QObject::connect(
        ui->doubleModeButton,
        &QToolButton::clicked,
        this,
        [this](){
            setMode(CalculationMode::Double);
            showLowerMatrix();
        });
    //single operation
    QObject::connect(
        ui->stepOperationComboBox,
        &QComboBox::currentIndexChanged,
        ui->singleParameterStack,
        [this](int index){
            ui->singleParameterStack->setCurrentIndex(index);
        });
    //operation set text
    QObject::connect(
        ui->operationComboBox,
        &QComboBox::currentIndexChanged,
        this,
        [this](int)
        {
            ui->relationLabel->setText(
                ui->operationComboBox->currentData().toString()
                );
        }
        );
    //calculate double
    QObject::connect(
        ui->calculateDoubleButton,
        &QPushButton::clicked,
        this,
        [this](){
            calculateDouble();
        });
    // connect step
    QObject::connect(
        ui->applyStepButton,
        &QPushButton::clicked,
        this,
        [this](){
            applySingleStep();
        });
    QObject::connect(
        ui->stepListWidget,
        &QListWidget::currentRowChanged,
        this,
        [this](int stepIndex)
        {
            showSingleStep(stepIndex);
        });

}

void CalculatePage::setRepository(const MatrixRepository *repository)
{
    repository_ = repository;
    refreshOptions();
}
void CalculatePage::refreshOptions()
{
    if(repository_ == nullptr)
    {
        return;
    }
    ui->upperMatrixComboBox->clear();
    ui->lowerMatrixComboBox->clear();
    for(const auto &item : repository_->matrixRepository())
    {
        ui->upperMatrixComboBox->addItem(item.name_,item.id_);
        ui->lowerMatrixComboBox->addItem(item.name_,item.id_);
    }

}
void CalculatePage::refreshOperationOptions()
{
    ui->operationComboBox->clear();
    if(mode_ == CalculationMode::Double)
    {
        ui->operationComboBox->addItem("Add", "+");
        ui->operationComboBox->addItem("Subtract", "−");
        ui->operationComboBox->addItem("Multiply", "×");
    }
    else if(mode_ == CalculationMode::Single)
    {
        ui->operationComboBox->addItem("REF");
        ui->operationComboBox->addItem("RREF");
    }
}
void CalculatePage::updateRowInputRanges(int rowCount)
{
    ui->swapFirstRowSpinBox->setMaximum(rowCount);
    ui->swapSecondRowSpinBox->setMaximum(rowCount);
    ui->addTargetRowSpinBox->setMaximum(rowCount);
    ui->addSourceRowSpinBox->setMaximum(rowCount);
    ui->scaleRowSpinBox->setMaximum(rowCount);
}



void CalculatePage::showUpperMatrix()
{
    if(repository_ == nullptr)
    {
        return;
    }
    if(ui->upperMatrixComboBox->currentIndex() < 0)
    {
        return;
    }
    QUuid id = ui->upperMatrixComboBox->currentData().toUuid();
    const MatrixRecord *record = repository_->find(id);
    if(record == nullptr)
    {
        return;
    }
    std::visit(
        [this](const auto &matrix){
            const int row = static_cast<int>(matrix.row());
            updateRowInputRanges(row);
        }, record->matrix_);
    ui->upperMatrixWidget->setMatrix(record->matrix_);
}

void CalculatePage::showLowerMatrix()
{
    if(repository_ == nullptr)
    {
        return;
    }
    if(ui->lowerMatrixComboBox->currentIndex() < 0)
    {
        return;
    }
    if(mode_ == CalculationMode::Single)
    {
        return;
    }
    QUuid id = ui->lowerMatrixComboBox->currentData().toUuid();
    const MatrixRecord *record = repository_->find(id);
    if(record == nullptr)
    {
        return;
    }
    ui->lowerMatrixWidget->setMatrix(record->matrix_);
}

void CalculatePage::setMode(CalculationMode mode)
{
    mode_ = mode;
    if(mode == CalculationMode::Single)
    {
        ui->lowerLabel->hide();
        ui->lowerMatrixComboBox->hide();
        ui->modePanelStack->setCurrentIndex(0);
        ui->lowerMatrixLabel->setText(QString("After Step"));
        ui->upperMatrixLabel->setText(QString("Before Step"));
    }
    else if(mode == CalculationMode::Double)
    {
        ui->lowerLabel->show();
        ui->lowerMatrixComboBox->show();
        ui->lowerMatrixLabel->show();
        ui->modePanelStack->setCurrentIndex(1);
        ui->lowerMatrixLabel->setText(QString("Matrix B"));
        ui->upperMatrixLabel->setText(QString("Matrix A"));
    }
    refreshOperationOptions();

}
void CalculatePage::calculateDouble()
{
    //get matrix
    if(repository_ == nullptr)
    {
        return;
    }
    if(ui->lowerMatrixComboBox->currentIndex() < 0 || ui->upperMatrixComboBox->currentIndex() < 0)
    {
        return;
    }
    QUuid idA = ui->upperMatrixComboBox->currentData().toUuid();
    QUuid idB = ui->lowerMatrixComboBox->currentData().toUuid();
    const MatrixRecord *recordA = repository_->find(idA);
    const MatrixRecord *recordB = repository_->find(idB);
    if(recordA == nullptr || recordB == nullptr)
    {
        return;
    }
    const MatrixVariant &MatrixA = recordA->matrix_;
    const MatrixVariant &MatrixB = recordB->matrix_;
    if(MatrixA.index() != MatrixB.index())
    {
        QMessageBox::warning(
            this,
            "Type Mismatch",
            "Sorry, only matrices of the same data type can be used together."
            );
        return;
    }
    const int operationIndex = ui->operationComboBox->currentIndex();
    //get row/column counts
    if(operationIndex < 0)
    {
        return;
    }
    auto visitrows = [](const auto &matrix){ return matrix.row();};
    auto visitcolumns =  [](const auto &matrix){ return matrix.column(); };
    const std::size_t rowsA = std::visit(visitrows, MatrixA);
    const std::size_t columnsA = std::visit( visitcolumns,MatrixA);
    const std::size_t rowsB = std::visit(visitrows, MatrixB);
    const std::size_t columnsB = std::visit( visitcolumns, MatrixB);
    if(operationIndex == 0 || operationIndex == 1)
    {
        if(rowsA !=rowsB || columnsA != columnsB)
        {
            QMessageBox::warning(
                this,
                "Dimension Mismatch",
                "Addition and subtraction require matrices with the same dimensions."
                );
            return;
        }
    }
    else if(operationIndex == 2)
    {
        if(columnsA != rowsB)
        {
            QMessageBox::warning(
                this,
                "Dimension Mismatch",
                "Matrix A's column count must equal Matrix B's row count."
                );
            return;
        }
    }
    auto calculator = [operationIndex,&MatrixB](const auto &matrixA) ->MatrixVariant
    {
        using MatrixType = std::decay_t<decltype(matrixA)>;
        const MatrixType &matrixB = std::get<MatrixType>(MatrixB);
        if(operationIndex == 0)
        {
            return matrixA + matrixB;
        }
        if(operationIndex == 1)
        {
            return matrixA - matrixB;
        }
        else
        {
            return matrixA * matrixB;
        }
    };
    MatrixVariant resultMatrix = std::visit(calculator , MatrixA);
    ui->resultMatrixWidget->setMatrix(resultMatrix);
}

void CalculatePage::applySingleStep()
{
    if(repository_ == nullptr)
    {
        return;
    }
    //get before matrix
    MatrixVariant beforeMatrix;
    if(steps_.empty())
    {
        QUuid id = ui->upperMatrixComboBox->currentData().toUuid();
        const MatrixRecord *record = repository_->find(id);
        if(record == nullptr)
        {
            return;
        }
        beforeMatrix = record->matrix_;
    }
    else
    {
        beforeMatrix = steps_.back().resultMatrix_;
    }
    //get operations
    RowOperation operation;
    const int index = ui->stepOperationComboBox->currentIndex();
        //swap rows
    if(index == 0)
    {
        //index
        int IfirstROwIndex = ui->swapFirstRowSpinBox->value()-1;
        int IsecondROwIndex = ui->swapSecondRowSpinBox->value()-1;
        std::size_t firstRowIndex = static_cast<std::size_t>(IfirstROwIndex);
        std::size_t secondRowIndex= static_cast<std::size_t>(IsecondROwIndex);
        if(firstRowIndex == secondRowIndex)
        {
            QMessageBox::warning(
                this,
                "paramater wrong",
                "useless to swap same row"
                );
            return;
        }
        operation = SwapRowsOperation{firstRowIndex, secondRowIndex};
    }
        //add scaled row operation
    else if(index == 1)
    {
        //index
        int IaddTargetRowSpinBox = ui->addTargetRowSpinBox->value()-1;
        int IaddSourceRowSpinBox = ui->addSourceRowSpinBox->value()-1;
        std::size_t targetRowIndex = static_cast<std::size_t>(IaddTargetRowSpinBox);
        std::size_t sourceRowIndex = static_cast<std::size_t>(IaddSourceRowSpinBox);
        if(targetRowIndex == sourceRowIndex)
        {
            QMessageBox::warning(
                this,
                "paramater wrong",
                "cannot be same row"
                );
            return;
        }
        //factor
        QString Sfactor = ui->addMultiplierLineEdit->text();
        std::optional<ScalarVariant> Ofactor = QStringToScalarVariant(Sfactor);
        if(Ofactor.has_value())
        {
            ScalarVariant factor = Ofactor.value();
            operation = AddScaledRowOperation {targetRowIndex, factor, sourceRowIndex};
        }
        else
        {
            return;
        }
    }
        //scale row operation
    else if(index == 2)
    {
        //index
        int IscaleRowSpinBox = ui->scaleRowSpinBox->value()-1;
        std::size_t rowIndex = static_cast<std::size_t>(IscaleRowSpinBox);
        //factor
        QString Sfactor = ui->scaleMultiplierLineEdit->text();
        std::optional<ScalarVariant> Ofactor = QStringToScalarVariant(Sfactor);
        if(Ofactor.has_value())
        {
            ScalarVariant factor = Ofactor.value();
            bool ok = std::visit(
                [this](const auto &factor) -> bool
                {
                    if(factor == 0)
                    {
                        QMessageBox::warning(
                            this,
                            "paramater wrong",
                            "cannot be zero"
                            );
                        return false;
                    }
                    return true;
                },
                factor);
            if(!ok){return;}
            operation = ScaleRowOperation {factor, rowIndex};
        }
        else
        {
            return;
        }
    }
    else
    {
        return;
    }
    //get result matrix
    auto visitor = [](auto matrix,const auto &operations) -> std::optional<MatrixVariant>
    {
        using OperationType = std::decay_t<decltype(operations)>;
        using MatrixType = std::decay_t<decltype(matrix)>;
        using ScalarType = std::decay_t<decltype(matrix.at(0,0))>;
        auto visitfactor = [](const auto factor) -> std::optional<ScalarType>
        {
            using FactorType = std::decay_t<decltype(factor)>;
            if constexpr (std::is_same_v<ScalarType,int>)
            {
                if constexpr (std::is_same_v<FactorType,int>)
                {
                    return int(factor);
                }
                else
                {
                    return std::nullopt;
                }
            }
            else if constexpr (std::is_same_v<ScalarType,double>)
            {
                if constexpr (std::is_same_v<FactorType,int>)
                {
                    return double(factor);
                }
                else if constexpr (std::is_same_v<FactorType,double>)
                {
                    return double(factor);
                }
                else if constexpr (std::is_same_v<FactorType,Rational>)
                {
                    return static_cast<double>(factor.numer()) / static_cast<double>(factor.denom());
                }
            }
            else if constexpr (std::is_same_v<ScalarType,Rational>)
            {
                if constexpr (std::is_same_v<FactorType,int>)
                {
                    return Rational(factor);
                }
                else if constexpr (std::is_same_v<FactorType,Rational>)
                {
                    return Rational(factor);
                }
                else
                {
                    return std::nullopt;
                }
            }
        };
        if constexpr (std::is_same_v<OperationType,SwapRowsOperation>)
        {
            matrix.swap_two_lines(operations.firstRowIndex_, operations.secondRowIndex_);
            return matrix;
        }
        else if constexpr(std::is_same_v<OperationType,AddScaledRowOperation>)
        {

            auto factor = std::visit(visitfactor,operations.factor_);
            if(factor.has_value())
            {
                matrix.add_one_to_another(factor.value(),operations.sourceRowIndex_,operations.targetRowIndex_);
                return std::optional<MatrixVariant> (matrix);
            }
            else
            {
                return std::nullopt;
            }
        }
        else if constexpr(std::is_same_v<OperationType,ScaleRowOperation>)
        {

            auto factor = std::visit(visitfactor,operations.factor_);
            if(factor.has_value())
            {
                matrix.multiply_one_line(factor.value(), operations.rowIndex_);
                return std::optional<MatrixVariant> (matrix);
            }
            else
            {
                return std::nullopt;
            }
        }
        else
        {
            return {};
        }
    };
    std::optional<MatrixVariant> resultMatrix = std::visit(visitor,beforeMatrix, operation);
    if(resultMatrix.has_value())
    {
        MatrixStep step = {operation, resultMatrix.value()};
        steps_.push_back(step);
        ui->stepListWidget->addItem( "Step " + QString::number(steps_.size()) + ": " + rowOperationToQString(operation));
        ui->stepListWidget->setCurrentRow( ui->stepListWidget->count() - 1 );
    }
    else
    {
        return;
    }
}
std::optional<ScalarVariant> CalculatePage::QStringToScalarVariant(const QString &Sfactor)
{
    ScalarVariant factor;
    bool IOK, DOK, ROK, NOK, DEOK;
    int Ifactor = Sfactor.toInt(&IOK);
    if(IOK)
    {
        return Ifactor;
    }
    double Dfactor = Sfactor.toDouble(&DOK);
    if(DOK && std::isfinite(Dfactor))
    {
        return Dfactor;
    }
    QStringList parts = Sfactor.split('/');
    if(parts.count() != 2)
    {
        return std::nullopt;
    }
    int numer = parts[0].toInt(&NOK);
    int denom = parts[1].toInt(&DEOK);
    if(denom == 0)
    {
        return std::nullopt;
    }
    ROK = DEOK && NOK;
    if(ROK)
    {
        Rational Rfactor = Rational{numer,denom};
        factor = Rfactor;
    }
    else
    {
        return std::nullopt;
    }
    return factor;
}
void CalculatePage::showSingleStep(int stepIndex)
{
    if( stepIndex < 0 )
    {
        return;
    }
    std::size_t Sindex = static_cast<std::size_t>(stepIndex);
    if(Sindex >= steps_.size())
    {
        return;
    }
    if(Sindex == 0)
    {
        showUpperMatrix();
        ui->lowerMatrixWidget->setMatrix(steps_[Sindex].resultMatrix_);
    }
    else
    {
        ui->upperMatrixWidget->setMatrix(steps_[Sindex-1].resultMatrix_);
        ui->lowerMatrixWidget->setMatrix(steps_[Sindex].resultMatrix_);
    }

}
QString CalculatePage::rowOperationToQString(const RowOperation &operation) const
{
    auto visitor = [this](const auto &opt) -> QString
    {
        using operationType = std::decay_t<decltype(opt)>;
        if constexpr (std::is_same_v<operationType, SwapRowsOperation>)
        {
            return QString("R") + QString::number(opt.firstRowIndex_+1) + QString(" ↔ ") + QString("R") + QString::number(opt.secondRowIndex_+1);
        }
        else if constexpr (std::is_same_v<operationType, AddScaledRowOperation>)
        {
            return QString("R") + QString::number(opt.targetRowIndex_+1) + QString(" + ") +QString("(")
                   + scalarToQString(opt.factor_)+ QString(")") + QString("R") + QString::number(opt.sourceRowIndex_+1);
        }
        else if constexpr (std::is_same_v<operationType, ScaleRowOperation>)
        {
            return QString("(") + scalarToQString(opt.factor_)+ QString(")") + QString("R") + QString::number(opt.rowIndex_+1);
        }
    };
    return std::visit(visitor,operation);
}
QString CalculatePage::scalarToQString(const ScalarVariant &factor) const
{
    auto visitor = [](const auto &value) -> QString
    {
        using ScalarType = std::decay_t<decltype(value)>;
        if constexpr (std::is_same_v<ScalarType, int>)
        {
            return QString::number(value);
        }
        else if constexpr (std::is_same_v<ScalarType, double>)
        {
            return QString::number(value);
        }
        else if constexpr (std::is_same_v<ScalarType, Rational>)
        {
            return QString::number(value.numer()) + "/" + QString::number(value.denom());
        }
    };
    return std::visit(visitor,factor);

}

CalculatePage::~CalculatePage()
{
    delete ui;
}
