#ifndef EXCELSCRIPTER_H
#define EXCELSCRIPTER_H

#include <QWidget>
#include <QAxObject>
#include <QFileInfo>
#include <QTimer>
#include <QProgressDialog>
#include <QDir>
#include <QRandomGenerator>
#include <QCompleter>
#include "gbkfileio.h"
#include <QProgressDialog>
#include <QTreeWidgetItem>

namespace Ui
{
    class ExcelScripter;
}

class ExcelScripter : public QWidget
{
    Q_OBJECT

public:
    explicit ExcelScripter(QWidget *parent = nullptr);
    ~ExcelScripter();
    QMap<int, QStringList> readExcel();
    void updateTree();
    void createScript();
    void itemClicked(QTreeWidgetItem *item, int column);
    void splitByTopo();
    void splitDone();
    void selectAllItem();
    void enableWidget();
    void disableWidget();
    void titleComboBoxTextChanged(const QString &text);
    void filterComboBoxTextChanged(const QString &text);

private:
    Ui::ExcelScripter *ui;
    int headColumnStart;
    int headColumnEnd;
    int headRow;
    QStringList headTitleList;
    QString moduleNum;
    QString moduleName;
    QStringList fileHistory;
    GBKFileIO fileIO;
    QMap<int, QList<QTreeWidgetItem*>> topoItemMap;
    QMap<QString,int> titleColumnMap;
    enum column
    {
        TestPointNum,
        TestPointTitle,
        Description,
        ItemText,
        IsSetColor,
        BelongsTopoNum,
    };

    int topoNum = 0;
    int usedRGBIdx = 0;
    int RGBList[15][3] = {
        {236, 173, 113},
        {236, 223, 113},
        {177, 236, 113},
        {127, 236, 113},
        {113, 236, 203},
        {113, 161, 236},
        {132, 113, 236},
        {217, 113, 236},
        {236, 113, 134},
        {232, 158, 255},
        {255, 158, 158},
        {177, 255, 158},
        {253, 255, 158},
        {158, 228, 255},
        {236, 113, 113},
    };
};

#endif // EXCELSCRIPTER_H
