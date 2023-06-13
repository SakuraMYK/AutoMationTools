
#include "mainwindow.h"
#include "ui_mainwindow.h"
MainWindow* MainWindow::instance = nullptr;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), stackedWidget(new QStackedWidget(this))
{

    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/icon/winTitle.ico"));

    testSuite = nullptr;
    logAnalysis = nullptr;
    debugInfo = new DebugInfo();

    // 连接菜单按钮到对应的槽函数
    connect(ui->action_Create_Test_Case_Suite, &QAction::triggered, this, [&]()
            {
            if (!testSuite)
            {
                testSuite = new TestSuite(this);
                stackedWidget->addWidget(testSuite);
            }
            stackedWidget->setCurrentWidget(testSuite);
            this->setCentralWidget(stackedWidget);
    });

    connect(ui->action_Log_Analysis, &QAction::triggered, this, [&]()
            {
                if (!logAnalysis)
                {
                    logAnalysis = new LogAnalysis(this);
                    stackedWidget->addWidget(logAnalysis);
                }
                stackedWidget->setCurrentWidget(logAnalysis);
                this->setCentralWidget(stackedWidget);
            });

    connect(ui->action_Debug, &QAction::triggered, this, [&]()
            {
            debugInfo->setWindowTitle("debug");
            debugInfo->show();
            debugInfo->raise();
    });

    instance = this;
    qInstallMessageHandler(&MainWindow::debugMessageHandler);

    connect(this, &MainWindow::sendDebugOutput, debugInfo, &DebugInfo::append);

}

MainWindow::~MainWindow()
{
    delete ui;
    delete stackedWidget;
    delete debugInfo;
}

void MainWindow::debugMessageHandler(QtMsgType /*type*/, const QMessageLogContext& /*context*/, const QString& msg)
{
    if (MainWindow::instance)
    {
        emit MainWindow::instance->sendDebugOutput(msg);
    }
}
