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
#include <QDesktopServices>
#include <QIcon>
#include <QAxObject>
#include <QAxWidget>
#include <QTimer>

namespace Ui
{
    class LogAnalysis;
}

class LogAnalysis : public QWidget
{
    Q_OBJECT


private:
    Ui::LogAnalysis *ui;
    QString historicalPath;
    QString exportExcelPath;
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
        RelatedFileList
    };

    enum FileOperator
    {
        OpenWithIE,
        OpenWithNotepad,
        OpenWithExplorer,
        OpenWithExplorerAndSelect,
    };

public:
    explicit LogAnalysis(QWidget *parent = nullptr);
    ~LogAnalysis();
    QMap<QString, QString> getTestSuiteLogInfo(const QString &filePath);
    QMap<QString, QString> getScriptLogInfo(const QString &filePath);
    QMap<QString, QStringList> getAllScriptInfoFromTestSuiteLog(const QString &xmlPath);
    QStringList getAllTestSuiteXML(const QString &dir);
    QMap<QString, QMap<QString, QVariant>> traverseDirCreateTstMap();
    QProgressDialog* progressDialog();
    QString readFile(const QString&filePath);

    void updateTreeWidget();
    void openFile(const QString &filePath, FileOperator fileOperator);
    void onTriggered(const QPoint &pos);
    void updateItemTotal();
    void sortItems(int column);
    void exportExcel();

};


#endif // LOGANALYSIS_H
