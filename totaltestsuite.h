#ifndef TOTALTESTSUITE_H
#define TOTALTESTSUITE_H

#include <QWidget>
#include <QFileDialog>
#include <QTimer>
#include <QDirIterator>
#include <QTreeWidgetItem>
#include "gbkfileio.h"

namespace Ui
{
    class TotalTestSuite;
}

class TotalTestSuite : public QWidget
{
    Q_OBJECT

public:
    explicit TotalTestSuite(QWidget *parent = nullptr);
    ~TotalTestSuite();
    QMap<QString, QStringList> searchTst();
    void updateTreeWidget();
    void itemChanged(QTreeWidgetItem *item, int column);
    void createTotalTst();
    bool isItemChangedFirstConnect;

private:
    Ui::TotalTestSuite *ui;
};

#endif // TOTALTESTSUITE_H
