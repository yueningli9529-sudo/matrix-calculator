#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);
    QObject::connect(
        ui->BuildButton,
        &QPushButton::clicked,
        [this]()
        {
            int row = ui->RowSpinBox->value();
            int column = ui->ColumnSpinBox->value();
            int datatype = ui->DataComboBox->currentIndex();
            switch(datatype){
            case 0:
            {
                this->editing_matrix_ = Matrix<Rational> (row,column);
                break;
            }
            case 1:
            {
                this->editing_matrix_ = Matrix<int> (row,column);
                break;
            }
            case 2:
            {
                this->editing_matrix_ = Matrix<double> (row,column);
                break;
            }
            default:
                break;
            };
            build_matrix();
        });
    QObject::connect(
        ui->buttonBox,
        &QDialogButtonBox::accepted,
        this,
        [this]()
        {
            load_input();
            accept();
        });
    QObject::connect(
        ui->ClearButton,
        &QPushButton::clicked,
        this,
        [this](){
            clear_layout(matrixLayout_);
            ui->BuildButton->clicked();
        });

    ui->BuildButton->clicked();

}
MatrixVariant Dialog::matrix() const {
    return editing_matrix_;
}
void Dialog::clear_layout(QLayout *layout)
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

void Dialog::build_matrix()
{
    if (matrixLayout_ == nullptr)
    {
        matrixLayout_ = new QGridLayout(this->ui->frame);
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
    std::visit(visitor,this->editing_matrix_);
}
void Dialog::load_input()
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
    std::visit(visitor,this->editing_matrix_);
}

Dialog::~Dialog()
{
    delete ui;
}
