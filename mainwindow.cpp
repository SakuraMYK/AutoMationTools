
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), stackedWidget(new QStackedWidget(this))
{
    ui->setupUi(this);

    testSuite = nullptr;
    logAnalysis = nullptr;

    // 连接菜单按钮到对应的槽函数
    connect(ui->action_Create_Test_Case_Suite, &QAction::triggered, this, [&]()
            {
        if (!testSuite)
        {
            testSuite = new TestSuite(this);
        }
        stackedWidget->addWidget(testSuite);
        stackedWidget->setCurrentWidget(testSuite);
        setCentralWidget(stackedWidget); });
    connect(ui->action_Log_Analysis, &QAction::triggered, this, [&]()
            {
        if (!logAnalysis)
        {
            logAnalysis = new LogAnalysis(this);
        }
        stackedWidget->addWidget(logAnalysis);
        stackedWidget->setCurrentWidget(logAnalysis);
        setCentralWidget(stackedWidget); });
}

MainWindow::~MainWindow()
{
    delete ui;
    delete stackedWidget;
}
