#include "inputpage.h"
#include "ui_inputpage.h"
#include <QListWidgetItem>
#include <QInputDialog>
#include <QMessageBox>
#include "MatrixRepository.h"
#include <cmath>

InputPage::InputPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::InputPage)
{
    ui->setupUi(this);
    ui->displayPageLayout->setAlignment( ui->matrixDisplayWidget, Qt::AlignCenter);
    ui->splitter->setSizes({750, 250});
    set_input_state(input_state_);

    //build
    QObject::connect(
        ui->BuildButton,
        &QPushButton::clicked,
        [this]()
        {
            if(input_state_ == InputState::Editing)
            {
                QMessageBox::StandardButton choice = QMessageBox::question(
                    this,
                    "Unsaved Matrix",
                    "The current matrix has not been saved. Discard it?",
                    QMessageBox::Discard | QMessageBox::Cancel,
                    QMessageBox::Cancel
                    );
                if (choice == QMessageBox::Cancel)
                {
                    return;
                }
            }
            int row = ui->RowSpinBox->value();
            int column = ui->ColumnSpinBox->value();
            int datatype = ui->DataTypeComboBox->currentIndex();
            current_record_ = MatrixRecord();
            switch(datatype){
            case 0:
            {
                this->current_record_.value().matrix_ = Matrix<Rational> (row,column);
                break;
            }
            case 1:
            {
                this->current_record_.value().matrix_ = Matrix<int> (row,column);
                break;
            }
            case 2:
            {
                this->current_record_.value().matrix_ = Matrix<double> (row,column);
                break;
            }
            default:
                break;
            };
            build_matrix();
            set_input_state(InputState::Editing);
        });
    //save
    QObject::connect(
        ui->SaveButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            std::optional<InputError> error = load_input();
            if(error.has_value())
            {
                show_input_error(error.value());
                return;
            }

            QString name = name_matrix();
            if(name.isEmpty())
            {
                return;
            }
            else
            {
                QUuid id = QUuid::createUuid();
                this->current_record_.value().id_ = id;
                this->current_record_.value().name_ = name;
            }
            demonstrate_current_matrix();
            emit save_button_clicked();
        });
    //switchtem
    QObject::connect(
        ui->repositoryListWidget,
        &QListWidget::itemClicked,
        this,
        [this](QListWidgetItem *item){
            if (item == nullptr || repository_ == nullptr)
            {
                return;
            }
            QVariant Qid = item->data(Qt::UserRole);
            QUuid id = Qid.toUuid();
            const MatrixRecord *record = repository_->find(id);
            if (record == nullptr)
            {
                return;
            }
            current_record_ = *record;
            demonstrate_current_matrix();
        });
    //search engine
    QObject::connect(
        ui->repositorySearchLineEdit,
        &QLineEdit::textChanged,
        this,
        [this](QString keyword){
            filter_repo_list(keyword);
        });
}
// state
void InputPage::set_input_state(InputState input_state)
{
    input_state_ = input_state;
    update_ui_for_state();
}

void InputPage::update_ui_for_state()
{
    if(input_state_ == InputState::Empty)
    {
        ui->matrixStackedWidget->setVisible(true);
        ui->matrixStackedWidget->setCurrentWidget(ui->emptyPage);
        ui->SaveButton->setEnabled(false);
    }
    else if(input_state_ == InputState::Editing)
    {
        ui->matrixStackedWidget->setVisible(true);
        ui->matrixStackedWidget->setCurrentWidget(ui->editorPage);
        ui->SaveButton->setEnabled(true);
    }
    else if(input_state_ == InputState::Viewing)
    {
        ui->matrixStackedWidget->setVisible(true);
        ui->matrixStackedWidget->setCurrentWidget(ui->displayPage);
        ui->SaveButton->setEnabled(false);
    }
}

MatrixRecord InputPage::InputResult() const
{
    return current_record_.value();
}
void InputPage::show_input_error(const InputError &error)
{
    if(error.input_error_type_ == InputErrorType::InvalidValue)
    {
        if(error.position_.has_value())
        {
            QString part;
            if(error.position_->input_part_ == InputPart::Numerator)
            {
                part = "numerator";
            }
            else if(error.position_->input_part_ == InputPart::Denominator)
            {
                part = "denominator";
            }
            else
            {
                part = "value";
            }
            QMessageBox::warning(
                this,
                "Invalid Input",
                QString("Invalid %1 at row %2, column %3.").arg(part)
                    .arg(error.position_->row_number_)
                    .arg(error.position_->column_number_)
                );
            return;
        }
        else
        {
            QMessageBox::warning(
                this,
                "Invalid Input",
                "The matrix contains an invalid value."
                );
            return;
        }
    }
    else
    {
        if(error.position_ == std::nullopt)
        {
            QMessageBox::critical(
                this,
                "Internal State Error",
                "Unable to read the matrix because of an internal error."
                );
            return;
        }
        else
        {
            QMessageBox::critical(
                this,
                "Internal State Error",
                QString("Unable to read the matrix because of an internal error at row %1, column %2.")
                    .arg(error.position_->row_number_)
                    .arg(error.position_->column_number_)
                );
            return;
        }
    }
}

void InputPage::setRepository( const MatrixRepository *repository)
{
    repository_ = repository;
}
void InputPage::refreshRepositoryList()
{
    ui->repositoryListWidget->clear();
    if(repository_ == nullptr)
    {
        return;
    }
    for(const auto &record : repository_->matrixRepository())
    {
        auto *item = new QListWidgetItem(record.name_);
        item->setData(Qt::UserRole, record.id_);
        ui->repositoryListWidget->addItem(item);
    }
}

//list widget
void InputPage::filter_repo_list(const QString keyword)
{
    QString tkeyword = keyword.trimmed();
    if(repository_ == nullptr)
    {
        return;
    }
    int count = ui->repositoryListWidget->count();
    for(int i = 0 ; i < count ; i++)
    {
        QListWidgetItem *item = ui->repositoryListWidget->item(i);
        if(item == nullptr)
        {
            return;
        }
        bool ok = item->text().contains(tkeyword , Qt::CaseInsensitive);
        item->setHidden(!ok);
    }
}

//input
void InputPage::clear_layout(QLayout *layout)
{
    if (layout == nullptr)
    {
        return;
    }
    while(QLayoutItem *item = layout->takeAt(0))
    {
        if (QWidget *widget = item->widget())
        {
            delete widget;  //delete QWidget
            delete item;    //delete QWidget Item
        }
        else if (QLayout *childLayout = item->layout())
        {
            clear_layout(childLayout);
            delete childLayout;
        }
        else
        {
            delete item;
        }
    }
}
void InputPage::build_matrix()
{
    ui->matrixStackedWidget->setCurrentWidget(ui->editorPage);

    if (matrixLayout_ == nullptr)
    {

        matrixLayout_ = new QGridLayout(this->ui->MatrixFrame);
        matrixLayout_->setHorizontalSpacing(25);
        matrixLayout_->setVerticalSpacing(15);
    }
    else
    {
        clear_layout(matrixLayout_);
    }

    auto visitor = [this](const auto &matrix)
    {
        using T = std::decay_t<decltype(matrix)>;

        if constexpr(std::is_same_v<T,Matrix<Rational>>)
        {
            for(int i = 0; i<matrix.row(); i++)
            {
                for(int j = 0; j<matrix.column(); j++){
                    QHBoxLayout *cell = new QHBoxLayout();
                    QLineEdit *numerator = new QLineEdit();
                    auto *numerator_validator = new QIntValidator(-999999,999999,numerator);
                    numerator->setValidator(numerator_validator);
                    numerator->setPlaceholderText(QString("0"));
                    numerator->setMaximumSize(50,20);
                    numerator->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);

                    QLabel *vinculum = new QLabel();
                    vinculum->setText(QString("/"));

                    QLineEdit *denominator = new QLineEdit();
                    auto *denominator_validator = new QIntValidator(1,9999,denominator);
                    denominator->setValidator(denominator_validator);
                    denominator->setPlaceholderText(QString("1"));
                    denominator->setMaximumSize(50,20);
                    denominator->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);

                    cell->addWidget(numerator);
                    cell->addWidget(vinculum);
                    cell->addWidget(denominator);
                    this->matrixLayout_->addLayout(cell,i,j);
                }

            }
        }
        else if constexpr(std::is_same_v<T,Matrix<int>>)
        {
            for(int i = 0; i<matrix.row(); i++)
            {
                for(int j = 0; j<matrix.column(); j++){
                    QHBoxLayout *cell = new QHBoxLayout();
                    QLineEdit *number = new QLineEdit();
                    auto *number_validator = new QIntValidator(-999999,999999,number);
                    number->setValidator(number_validator);
                    number->setPlaceholderText(QString("0"));

                    cell->addWidget(number);
                    this->matrixLayout_->addLayout(cell,i,j);
                }

            }
        }
        else if constexpr(std::is_same_v<T,Matrix<double>>)
        {
            for(int i = 0; i<matrix.row(); i++)
            {
                for(int j = 0; j<matrix.column(); j++){
                    QHBoxLayout *cell = new QHBoxLayout();
                    QLineEdit *number = new QLineEdit();
                    auto *number_validator = new QDoubleValidator(-999999,999999,2,number);
                    number->setValidator(number_validator);
                    number->setPlaceholderText(QString("0.00"));

                    cell->addWidget(number);
                    this->matrixLayout_->addLayout(cell,i,j);
                }

            }
        }
    };
    std::visit(visitor,this->current_record_.value().matrix_);
}
std::optional<InputPage::InputError> InputPage::load_input()
{
    if(input_state_ != InputState::Editing)
    {
        return InputError{InputErrorType::InternalStateError,std::nullopt};
    }

    if(!current_record_.has_value() || matrixLayout_ == nullptr)
    {
        return InputError{InputErrorType::InternalStateError,std::nullopt};
    }
    auto visitor = [this](auto &matrix) -> std::optional<InputError>
    {
        using T = std::decay_t<decltype(matrix)>;
        if constexpr (std::is_same_v<T,Matrix<Rational>>)
        {
            for(std::size_t i=0 ;i<matrix.row(); i++){
                for(std::size_t j=0 ;j<matrix.column(); j++){
                    QGridLayout *mLayout = this->matrixLayout_;
                    QLayoutItem *mlayoutitem = mLayout->itemAtPosition(i,j);
                    if(mlayoutitem == nullptr)
                    {return InputError{InputErrorType::InternalStateError,InputPosition{i+1,j+1,InputPart::Numerator}};}
                    QLayout *HLayout = mlayoutitem->layout();
                    if(HLayout == nullptr)
                    {return InputError{InputErrorType::InternalStateError,InputPosition{i+1,j+1,InputPart::Numerator}};}
                    QLayoutItem *item1 = HLayout->itemAt(0);
                    if(item1 == nullptr)
                    {return InputError{InputErrorType::InternalStateError,InputPosition{i+1,j+1,InputPart::Numerator}};}
                    QWidget *w1 = item1->widget();
                    if(w1 == nullptr)
                    {return InputError{InputErrorType::InternalStateError,InputPosition{i+1,j+1,InputPart::Numerator}};}
                    QLineEdit *lineEdit1 = qobject_cast<QLineEdit *>(w1);
                    if (lineEdit1 == nullptr)
                    {return InputError{InputErrorType::InternalStateError,InputPosition{i+1,j+1,InputPart::Numerator}};}
                    QString text1 = lineEdit1->text();
                    bool nok = true;
                    int numerator = 0;
                    if(!text1.isEmpty())
                    {
                        if (!lineEdit1->hasAcceptableInput())
                        {return InputError{InputErrorType::InvalidValue,InputPosition{i+1,j+1,InputPart::Numerator}};}
                        numerator = text1.toInt(&nok);
                    }
                    if(!nok)
                    {return InputError{InputErrorType::InvalidValue,InputPosition{i+1,j+1,InputPart::Numerator}};}


                    QLayoutItem *item2 = HLayout->itemAt(2);
                    if(item2 == nullptr)
                    {return InputError{InputErrorType::InternalStateError,InputPosition{i+1,j+1,InputPart::Denominator}};}
                    QWidget *w2 = item2->widget();
                    if(w2 == nullptr)
                    {return InputError{InputErrorType::InternalStateError,InputPosition{i+1,j+1,InputPart::Denominator}};}
                    QLineEdit *lineEdit2 = qobject_cast<QLineEdit *>(w2);
                    if (lineEdit2 == nullptr)
                    {return InputError{InputErrorType::InternalStateError,InputPosition{i+1,j+1,InputPart::Denominator}};}
                    QString text2 = lineEdit2->text();
                    bool dok = true;
                    int denominator = 1;
                    if(!text2.isEmpty())
                    {
                        if (!lineEdit2->hasAcceptableInput())
                        {return InputError{InputErrorType::InvalidValue,InputPosition{i+1,j+1,InputPart::Denominator}};}
                        denominator = text2.toInt(&dok);
                    }
                    if(!dok || denominator == 0)
                    {return InputError{InputErrorType::InvalidValue,InputPosition{i+1,j+1,InputPart::Denominator}};}
                    Rational rational = Rational(numerator,denominator);
                    matrix.at(i,j)= rational;
                }
            }
            return std::nullopt;
        }
        else if constexpr (std::is_same_v<T,Matrix<int>>)
        {
            for(std::size_t i=0 ;i<matrix.row(); i++){
                for(std::size_t j=0 ;j<matrix.column(); j++){
                    QLayoutItem *cellItem = matrixLayout_->itemAtPosition(i,j);
                    if(cellItem == nullptr)
                    {return InputError{InputErrorType::InternalStateError,InputPosition{i+1,j+1,InputPart::Value}};}
                    QLayout *HLayout = cellItem->layout();
                    if(HLayout == nullptr)
                    {return InputError{InputErrorType::InternalStateError,InputPosition{i+1,j+1,InputPart::Value}};}
                    QLayoutItem *inputItem = HLayout->itemAt(0);
                    if(inputItem == nullptr)
                    {return InputError{InputErrorType::InternalStateError,InputPosition{i+1,j+1,InputPart::Value}};}
                    QLineEdit *lineEdit1 = qobject_cast<QLineEdit *>(inputItem->widget());
                    if (lineEdit1 == nullptr)
                    {return InputError{InputErrorType::InternalStateError,InputPosition{i+1,j+1,InputPart::Value}};}
                    QString text1 = lineEdit1->text();
                    bool ok = true;
                    int number = 0;
                    if(!text1.isEmpty())
                    {
                        if (!lineEdit1->hasAcceptableInput())
                        {return InputError{InputErrorType::InvalidValue,InputPosition{i+1,j+1,InputPart::Value}};}
                        number = text1.toInt(&ok);
                    }
                    if(!ok)
                    {return InputError{InputErrorType::InvalidValue,InputPosition{i+1,j+1,InputPart::Value}};}
                    matrix.at(i,j)= number;
                }
            }
            return std::nullopt;
        }
        else if constexpr (std::is_same_v<T,Matrix<double>>)
        {
            for(std::size_t i=0 ;i<matrix.row(); i++){
                for(std::size_t j=0 ;j<matrix.column(); j++){
                    QLayoutItem *cellItem = matrixLayout_->itemAtPosition(i,j);
                    if(cellItem == nullptr)
                    {return InputError{InputErrorType::InternalStateError,InputPosition{i+1,j+1,InputPart::Value}};}
                    QLayout *HLayout = cellItem->layout();
                    if(HLayout == nullptr)
                    {return InputError{InputErrorType::InternalStateError,InputPosition{i+1,j+1,InputPart::Value}};}
                    QLayoutItem *inputItem = HLayout->itemAt(0);
                    if(inputItem == nullptr)
                    {return InputError{InputErrorType::InternalStateError,InputPosition{i+1,j+1,InputPart::Value}};}
                    QLineEdit *lineEdit1 = qobject_cast<QLineEdit *>(inputItem->widget());
                    if (lineEdit1 == nullptr)
                    {return InputError{InputErrorType::InternalStateError,InputPosition{i+1,j+1,InputPart::Value}};}
                    QString text1 = lineEdit1->text();
                    bool ok = true;
                    double number = 0;
                    if(!text1.isEmpty())
                    {
                        if (!lineEdit1->hasAcceptableInput())
                        {return InputError{InputErrorType::InvalidValue,InputPosition{i+1,j+1,InputPart::Value}};}
                        number = text1.toDouble(&ok);
                    }
                    if(!ok || !std::isfinite(number))
                    {return InputError{InputErrorType::InvalidValue,InputPosition{i+1,j+1,InputPart::Value}};}
                    matrix.at(i,j)= number;
                }
            }
            return std::nullopt;
        }
    };
    MatrixVariant temporary_matrix = current_record_.value().matrix_;
    std::optional<InputError> error = std::visit(visitor,temporary_matrix);
    if(error.has_value())
    {
        return error;
    }
    else
    {
        current_record_.value().matrix_ = temporary_matrix;
        return std::nullopt;
    }
}
void InputPage::demonstrate_current_matrix()
{
    ui->matrixDisplayWidget->setMatrix(current_record_.value().matrix_);
    ui->matrixStackedWidget->setCurrentWidget(ui->displayPage);
}
QString InputPage::name_matrix()
{
    bool ok = false;

    QString name = QInputDialog::getText(
        this,
        "Name matrix",
        "Matrix name:",
        QLineEdit::Normal,
        QString("Matrix"),
        &ok
        );

    if (!ok)
    {
        return {};
    }
    name = name.trimmed();
    if (name.isEmpty())
    {
        return {};
    }
    return name;
}
InputPage::~InputPage()
{
    delete ui;
}
