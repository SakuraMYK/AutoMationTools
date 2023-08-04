
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QLabel>
#include <QGroupBox>

MainWindow *MainWindow::instance = nullptr;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), stackedWidget(new QStackedWidget(this))
{

    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/icon/winTitle.ico"));

    testSuite = nullptr;
    totalTestSuite = nullptr;
    logAnalysis = nullptr;
    excelScripter = nullptr;
    debugInfo = new DebugInfo();
    hwnd = nullptr;

    // 连接菜单按钮到对应的槽函数
    connect(ui->actionCreate_Test_Case_Suite, &QAction::triggered, this, [&]()
            {
            if (!testSuite)
            {
                testSuite = new TestSuite(this);
                stackedWidget->addWidget(testSuite);
            }
            stackedWidget->setCurrentWidget(testSuite);
            this->setCentralWidget(stackedWidget); });

    connect(ui->actionCreate_Total_Test_Case_Suite, &QAction::triggered, this, [&]()
            {
                if (!totalTestSuite)
                {
                    totalTestSuite = new TotalTestSuite(this);
                    stackedWidget->addWidget(totalTestSuite);
                }
                stackedWidget->setCurrentWidget(totalTestSuite);
                this->setCentralWidget(stackedWidget); });

    connect(ui->actionLog_Analysis, &QAction::triggered, this, [&]()
            {
                if (!logAnalysis)
                {
                    logAnalysis = new LogAnalysis(this);
                    stackedWidget->addWidget(logAnalysis);
                }
                stackedWidget->setCurrentWidget(logAnalysis);
                this->setCentralWidget(stackedWidget); });

    connect(ui->actionCreate_Scripts_From_Excel, &QAction::triggered, this, [&]()
            {
                if (!excelScripter)
                {
                    excelScripter = new ExcelScripter(this);
                    stackedWidget->addWidget(excelScripter);
                }
                stackedWidget->setCurrentWidget(excelScripter);
                this->setCentralWidget(stackedWidget); });

    connect(ui->actionWindows, &QAction::triggered, this, [&]()
            {
                if (!hwnd)
                {
                    hwnd = new WindowsHWND(this);
                    stackedWidget->addWidget(hwnd);
                }
                stackedWidget->setCurrentWidget(hwnd);
                this->setCentralWidget(stackedWidget); });

    connect(ui->action_Debug, &QAction::triggered, this, [&]()
            {
            debugInfo->setWindowTitle("debug");
            debugInfo->show();
            debugInfo->raise(); });

    instance = this;
    //    qInstallMessageHandler(&MainWindow::debugMessageHandler);

    connect(this, &MainWindow::sendDebugOutput, debugInfo, &DebugInfo::append);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete stackedWidget;
    delete debugInfo;
}

void MainWindow::debugMessageHandler(QtMsgType /*type*/, const QMessageLogContext & /*context*/, const QString &msg)
{
    if (MainWindow::instance)
    {
        emit MainWindow::instance->sendDebugOutput(msg);
    }
}
