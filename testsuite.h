#ifndef TESTSUITE_H
#define TESTSUITE_H

#include <QWidget>
#include <QTreeWidgetItem>
#include <QFileInfo>
#include <QScrollBar>
#include <QTimer>
#include <QFileDialog>
#include <gbkfileio.h>
#include <QDirIterator>
#include <QTimer>

namespace Ui
{
    class TestSuite;
}

class TestSuite : public QWidget
{
    Q_OBJECT

public:
    explicit TestSuite(QWidget *parent = nullptr);
    ~TestSuite();
    void itemChanged(QTreeWidgetItem *item, int column);
    void itemClicked(QTreeWidgetItem *item, int column);
    void itemEntered(QTreeWidgetItem *item, int column);
    void updateSelectFileTip();
    void initCheckBox();
    void initLineEdit();
    void initComboBox();
    void initTreeWidget();
    void initPushButton();
    void createTotalTestSuite();
    void createTestSuite();
    void SearchFiles_and_UpdateTreeWidget();

private:
    Ui::TestSuite *ui;
    QTimer *timer;
    QString historicalPath;
};

#endif // TESTSUITE_H
