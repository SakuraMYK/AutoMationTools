#include "totaltestsuite.h"
#include "ui_totaltestsuite.h"

TotalTestSuite::TotalTestSuite(QWidget *parent) : QWidget(parent),
                                                  ui(new Ui::TotalTestSuite)
{
    ui->setupUi(this);

    connect(ui->pushButton_SelectDir, &QPushButton::clicked, this, [=]()
            { ui->lineEdit_Dir->setText(QFileDialog::getExistingDirectory(this, "选择目录", "", QFileDialog::ShowDirsOnly)); });

    connect(ui->pushButton_Search, &QPushButton::clicked, this, &TotalTestSuite::updateTreeWidget);

    connect(ui->pushButton_Create, &QPushButton::clicked, this, &TotalTestSuite::createTotalTst);

    ui->lineEdit_Dir->setText("d:\\log");
    ui->treeWidget_SearchReslut->headerItem()->setHidden(true);
    ui->label_InfoTip->setTextInteractionFlags(Qt::TextSelectableByKeyboard | Qt::TextSelectableByMouse);
    isItemChangedFirstConnect = true;
}

TotalTestSuite::~TotalTestSuite()
{
    delete ui;
}

QMap<QString, QStringList> TotalTestSuite::searchTst()
{
    QDir dir(ui->lineEdit_Dir->text());
    dir.setFilter(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files);
    QDirIterator it(dir, QDirIterator::Subdirectories);
    QMap<QString, QStringList> map;

    while (it.hasNext())
    {
        QString filePath = it.next();

        if (it.fileInfo().isFile() && it.fileInfo().suffix() == "tst")
        {
            map[it.fileInfo().dir().absolutePath()] << filePath;
        }
    }
    return map;
}

// 查找目录中所有符合条件的文件，并更新在控件上
void TotalTestSuite::updateTreeWidget()
{
    ui->treeWidget_SearchReslut->clear();

    QMap<QString, QStringList> map = searchTst();

    if (map.isEmpty())
    {
        return;
    }

    QStringList keyList = map.keys();
    for (const QString &key : keyList)
    {

        QStringList fileList = map[key];
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget_SearchReslut);
        item->setText(0, key);
        item->setCheckState(0, Qt::Checked);
        item->setIcon(0, QIcon(":/icon/folder.ico"));
        item->setData(0, Qt::UserRole, "parent");

        for (const QString &file : fileList)
        {
            QTreeWidgetItem *subItem = new QTreeWidgetItem(item);
            QFileInfo fileInfo(file);
            subItem->setText(0, fileInfo.fileName());
            subItem->setToolTip(0, fileInfo.absoluteFilePath());
            subItem->setCheckState(0, Qt::Checked);
            subItem->setIcon(0, QIcon(":/icon/icons8-file-64.png"));
            subItem->setData(0, Qt::UserRole, "child");
        }
    }

    if (isItemChangedFirstConnect)
    {
        connect(ui->treeWidget_SearchReslut, &QTreeWidget::itemChanged, this, &TotalTestSuite::itemChanged);
        isItemChangedFirstConnect = false;
    }
}

// 根据 checkState 更新勾选项
void TotalTestSuite::itemChanged(QTreeWidgetItem *item, int column)
{

    if (item == nullptr)
    {
        return;
    }

    if (item->data(column, Qt::UserRole).toString() == "parent")
    {

        switch (item->checkState(column))
        {
        case Qt::Checked:

            for (int idx = 0; idx < item->childCount(); ++idx)
            {
                item->child(idx)->setCheckState(column, Qt::Checked);
            }
            break;
        case Qt::Unchecked:

            for (int idx = 0; idx < item->childCount(); ++idx)
            {
                item->child(idx)->setCheckState(column, Qt::Unchecked);
            }
            break;

        default:
            break;
        }
    }
    else
    {
        if (item->parent() == nullptr)
        {
            qDebug() << __func__ << "Warning: " << item << "no parent object exists";
            return;
        }

        // 遍历所有子项，更新父控件的状态
        int childCount = item->parent()->childCount();
        int count = childCount;

        for (int idx = 0; idx < childCount; ++idx)
        {
            if (item->parent()->child(idx)->checkState(column) == Qt::Unchecked)
            {
                --count;
            }
        }

        // 更新父控件状态
        if (count == childCount && item->parent()->checkState(column) != Qt::Checked)
        {
            item->parent()->setCheckState(column, Qt::Checked);
        }
        else if (count == 0 && item->parent()->checkState(column) != Qt::Unchecked)
        {
            item->parent()->setCheckState(column, Qt::Unchecked);
        }
        else
        {
            item->parent()->setCheckState(column, Qt::PartiallyChecked);
        }
    }
}

void TotalTestSuite::createTotalTst()
{
    QStringList fileList;

    for (int idx = 0; idx < ui->treeWidget_SearchReslut->topLevelItemCount(); ++idx)
    {
        QTreeWidgetItem *item(ui->treeWidget_SearchReslut->topLevelItem(idx));
        for (int subIdx = 0; subIdx < item->childCount(); ++subIdx)
        {
            QTreeWidgetItem *subItem(item->child(subIdx));
            fileList << subItem->toolTip(0);
        }
    }

    if (fileList.isEmpty())
    {
        qDebug() << __func__ << "Warning: "
                 << "No tst file found";
    }
    else
    {
        GBKFileIO io;
        QString writePath = ui->lineEdit_Dir->text() + "/Main.tst";
        QString content;

        for (const QString &path : fileList)
        {
            content.append("<SOURCE> \"" + path + "\"\n");
        }

        io.writeFile(writePath, content);

        ui->label_InfoTip->setText("创建成功: " + writePath);
    }
}
