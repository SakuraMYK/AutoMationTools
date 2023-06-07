#ifndef LOGANALYSIS_H
#define LOGANALYSIS_H

#include <QWidget>
#include <QTreeWidgetItem>
#include <QMenu>
#include <QFileDialog>
#include <QDirIterator>
#include <QTextEdit>
#include <QProgressDialog>
#include <windows.h>
namespace Ui
{
    class LogAnalysis;
}

class LogAnalysis : public QWidget
{
    Q_OBJECT

public:
    explicit LogAnalysis(QWidget *parent = nullptr);
    ~LogAnalysis();
    void addOtherLogfileContextMenuActionsToItem(const QPoint &pos);
    void onAddItemTriggered(QAction *action);
    QString LongestCommonSubstring(QString &a, QString &b);
    void updateTreeWidget();
    QMap<QString,QString> getTestSuiteInfo(const QString&filePath);
    QMap<QString,QString> getScriptLogContent(const QString&filePath);
    QStringList getAllTclFromTestSuite(const QString &xmlPath);
    QStringList getAllTestSuiteXML(const QString &dir);
    void openXML(const QString&xml);
    void onTriggered(const QPoint &pos);

private:
    Ui::LogAnalysis *ui;
    QString historicalPath;
    bool isExpandAll;
};

#endif // LOGANALYSIS_H
