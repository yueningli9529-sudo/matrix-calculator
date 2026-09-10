#include "inputpage.h"
#include "ui_inputpage.h"
#include <QListWidgetItem>
#include <QInputDialog>
#include "MatrixRepository.h"

InputPage::InputPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::InputPage)
{
    ui->setupUi(this);
    ui->displayPageLayout->setAlignment( ui->matrixDisplayWidget, Qt::AlignCenter);
    ui->splitter->setSizes({750, 250});

    //build
    QObject::connect(
        ui->BuildButton,
        &QPushButton::clicked,
        [this]()
        {
            int row = ui->RowSpinBox->value();
            int column = ui->ColumnSpinBox->value();
            int datatype = ui->DataTypeComboBox->currentIndex();
            switch(datatype){
            case 0:
            {
                this->current_record_.matrix_ = Matrix<Rational> (row,column);
                break;
            }
            case 1:
            {
                this->current_record_.matrix_ = Matrix<int> (row,column);
                break;
            }
            case 2:
            {
                this->current_record_.matrix_ = Matrix<double> (row,column);
                break;
            }
            default:
                break;
            };
            build_matrix();
        });
    //save
    QObject::connect(
        ui->SaveButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            load_input();
            QString name = name_matrix();
            if(name.isEmpty())
            {
                return;
            }
            else
            {
                QUuid id = QUuid::createUuid();
                this->current_record_.id_ = id;
                this->current_record_.name_ = name;
            }
            switch_input_to_demonstrate();
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
            switch_input_to_demonstrate();
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

MatrixRecord InputPage::InputResult() const
{
    return current_record_;
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
                    auto *denominator_validator = new QIntValidator(1,999,denominator);
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
    std::visit(visitor,this->current_record_.matrix_);
}
void InputPage::load_input()
{
    auto visitor = [this](auto &matrix)
    {
        using T = std::decay_t<decltype(matrix)>;
        if constexpr (std::is_same_v<T,Matrix<Rational>>)
        {
            for(std::size_t i=0 ;i<matrix.row(); i++){
                for(std::size_t j=0 ;j<matrix.column(); j++){
                    QLayout *HLayout = this->matrixLayout_->itemAtPosition(i,j)->layout();
                    QWidget *w1 = HLayout->itemAt(0)->widget();
                    QLineEdit *lineEdit1 = qobject_cast<QLineEdit *>(w1);
                    if (lineEdit1 == nullptr){return;}
                    QString text1 = lineEdit1->text();
                    int numerator = text1.toInt();

                    QWidget *w2 = HLayout->itemAt(2)->widget();
                    QLineEdit *lineEdit2 = qobject_cast<QLineEdit *>(w2);
                    if (lineEdit2 == nullptr){return;}
                    QString text2 = lineEdit2->text();
                    if(text2.isEmpty()){text2 = QString("1");};
                    int denominator = text2.toInt();

                    Rational rational = Rational(numerator,denominator);
                    matrix.at(i,j)= rational;
                }
            }
        }
        else if constexpr (std::is_same_v<T,Matrix<int>>)
        {
            for(std::size_t i=0 ;i<matrix.row(); i++){
                for(std::size_t j=0 ;j<matrix.column(); j++){
                    QLayout *HLayout = this->matrixLayout_->itemAtPosition(i,j)->layout();
                    QWidget *w1 = HLayout->itemAt(0)->widget();
                    QLineEdit *lineEdit1 = qobject_cast<QLineEdit *>(w1);
                    if (lineEdit1 == nullptr){return;}
                    QString text1 = lineEdit1->text();
                    int number = text1.toInt();

                    matrix.at(i,j)= number;
                }
            }
        }
        else if constexpr (std::is_same_v<T,Matrix<double>>)
        {
            for(std::size_t i=0 ;i<matrix.row(); i++){
                for(std::size_t j=0 ;j<matrix.column(); j++){
                    QLayout *HLayout = this->matrixLayout_->itemAtPosition(i,j)->layout();
                    QWidget *w1 = HLayout->itemAt(0)->widget();
                    QLineEdit *lineEdit1 = qobject_cast<QLineEdit *>(w1);
                    if (lineEdit1 == nullptr){return;}
                    QString text1 = lineEdit1->text();
                    double number = text1.toDouble();

                    matrix.at(i,j)= number;
                }
            }
        }
    };
    std::visit(visitor,this->current_record_.matrix_);
}
void InputPage::switch_input_to_demonstrate()
{
    ui->matrixDisplayWidget->setMatrix(current_record_.matrix_);
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
