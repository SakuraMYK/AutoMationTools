
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include "testsuite.h"
#include "loganalysis.h"
#include "debuginfo.h"
#include "totaltestsuite.h"
#include "excelscripter.h"
#include "windowshwnd.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow

{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    static void debugMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

private:
    Ui::MainWindow *ui;
    QStackedWidget *stackedWidget;
    TestSuite *testSuite;
    TotalTestSuite *totalTestSuite;
    LogAnalysis *logAnalysis;
    DebugInfo *debugInfo;
    ExcelScripter *excelScripter;
    WindowsHWND *hwnd;
    static MainWindow *instance;

signals:
    void sendDebugOutput(const QString &message);
};

#endif // MAINWINDOW_H
