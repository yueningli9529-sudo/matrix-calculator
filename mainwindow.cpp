#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "pages/inputpage.h"
#include "pages/calculatepage.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    IPage = new InputPage();
    IPage->setRepository(&repository_);
    CPage = new CalculatePage();
    CPage->setRepository(&repository_);
    ui->stackedWidget->addWidget(IPage);
    ui->stackedWidget->addWidget(CPage);


    // connect pages
    QObject::connect(
        ui->InputToolButton,
        &QToolButton::toggled,
        this,
        [this](bool choosed)
        {
            if(choosed){ ui->stackedWidget->setCurrentWidget(IPage); }
        });
    QObject::connect(
        ui->CalculateToolButton,
        &QToolButton::toggled,
        this,
        [this](bool choosed){
            if(choosed)
            {
                ui->stackedWidget->setCurrentWidget(CPage);
                CPage->refreshOptions();
            }
        });

    //archive
    QObject::connect(
        ui->actionLoad_Archive,
        &QAction::triggered,
        this,
        [this](){
            auto loaded = MatrixSaveSystem::load();
            if (!loaded.has_value())
            {
                return;
            }

            repository_ = loaded.value();
            IPage->refreshRepositoryList();
        });
    QObject::connect(
        ui->actionSave_Archive,
        &QAction::triggered,
        this,
        [this](){
            MatrixSaveSystem::save(this->repository_);
        });
    //input page
    QObject::connect(
        this->IPage,
        &InputPage::save_button_clicked,
        this,
        [this](){
            if(repository_.add(this->IPage->InputResult()))
            {
                this->IPage->refreshRepositoryList();
                this->CPage->refreshOptions();
            }

        });
}

MainWindow::~MainWindow()
{
    delete ui;
}
