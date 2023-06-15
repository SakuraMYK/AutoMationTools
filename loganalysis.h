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
#include <QByteArray>
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
    void updateTreeWidget();
    QMap<QString, QString> getTestSuiteLogInfo(const QString &filePath);
    QMap<QString, QString> getScriptLogInfo(const QString &filePath);
    QMap<QString, QStringList> getAllScriptInfoFromTestSuiteLog(const QString &xmlPath);
    QStringList getAllTestSuiteXML(const QString &dir);
    void openXML(const QString &xmlPath);
    void onTriggered(const QPoint &pos);
    QMap<QString, QMap<QString, QVariant>> traverseDirCreateTstMap();
    QProgressDialog* progressDialog();
    void updateItemTotal();
    void sortItems(int column);



private:
    Ui::LogAnalysis *ui;
    QString historicalPath;
    bool isExpandAll;
    int progressDialog_Count;
    float total_UseTime;
    int total_OK;
    int total_NG;
    int total_ER;
    int total_InvalidHead;
    short int len_itemColumn;
    short int total_TestSuiteNum;
    bool sortOrder = true;
    enum column
    {
        TestSuiteName,
        StartTime,
        UseTime,
        TotalNum,
        OK,
        NG,
        ER,
        InvalidHead,
        PassRate,
        CoreFile,
        ID,
        XmlPath,
        TMXmlPath,
    };
};


#endif // LOGANALYSIS_H
