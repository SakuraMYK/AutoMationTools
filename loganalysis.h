#ifndef LOGANALYSIS_H
#define LOGANALYSIS_H

#include <QWidget>
#include <QTreeWidgetItem>
#include <QMenu>
#include <QFileDialog>
#include <QDirIterator>
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
    QString getTestSuiteName(QStringList &fileList);
    QString LongestCommonSubstring(QString &a, QString &b);
    void updateTreeWidget();
    QMap<QString,QMap<QString,QVariant>> getAllLogFiles();

private:
    Ui::LogAnalysis *ui;
    QString historicalPath;
};

#endif // LOGANALYSIS_H
