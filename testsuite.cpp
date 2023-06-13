#include "testsuite.h"
#include "ui_testsuite.h"

TestSuite::TestSuite(QWidget *parent)
    : QWidget(parent), ui(new Ui::TestSuite)
{
    ui->setupUi(this);
    initLineEdit();
    initComboBox();
    initTimer();
    initTreeWidget();
    initPushButton();
    initCheckBox();
}

TestSuite::~TestSuite()
{
    delete ui;
    delete timer;
}

void TestSuite::itemChanged(QTreeWidgetItem *item, int column)
{
    QFileInfo fileInfo(item->text(column));
    if (fileInfo.isDir())
    {
        if (item->checkState(column) == Qt::Checked)
        {
            for (int i = 0; i < item->childCount(); ++i)
            {
                item->child(i)->setCheckState(0, Qt::Checked);
            }
        }
        else if (item->checkState(column) == Qt::PartiallyChecked)
        {
            if (item->childCount() > 0)
            {
                short int childCheckedCount = 0;
                for (int i = 0; i < item->childCount(); ++i)
                {
                    if (item->child(i)->checkState(0) == Qt::Unchecked)
                    {
                        ++childCheckedCount;
                    }
                }
                if (childCheckedCount == item->childCount())
                {
                    item->setCheckState(column, Qt::Unchecked);
                }
            }
        }
        else if (item->checkState(column) == Qt::Unchecked)
        {
            for (int i = 0; i < item->childCount(); ++i)
            {
                item->child(i)->setCheckState(0, Qt::Unchecked);
            }
        }
        else
        {
            qDebug() << "未知类型错误!";
        }
    }
    else
    {
        short int childCheckedCount = 0;
        if (item->parent() != NULL)
        {
            // 如果点击修改的是子控件的状态，则遍历这个父控件下的所有子控件
            for (int i = 0; i < item->parent()->childCount(); ++i)
            {
                if (item->parent()->child(i)->checkState(0) == Qt::Checked)
                {
                    ++childCheckedCount;
                }
            }
            if (childCheckedCount == item->parent()->childCount())
            {
                item->parent()->setCheckState(0, Qt::Checked);
            }
            else
            {
                item->parent()->setCheckState(0, Qt::PartiallyChecked);
            }
        }
        else
        {
            //            qDebug() << "item:" << item << "column:" <<column << "has no parent object";
        }
    }
    updateSelectFileTip();
}

void TestSuite::itemClicked(QTreeWidgetItem *item, int column)
{
    QFileInfo file(item->text(column));
    // 不是文件夹，那就是进行点击切
    if (!file.isDir())
    {
        if (item->checkState(column) == Qt::Unchecked)
        {
            qDebug() << "change to Checked";
            item->setCheckState(column, Qt::Checked);
        }
        else
        {
            qDebug() << "change to Unchecked";
            item->setCheckState(column, Qt::Unchecked);
        }
    }
    else
    {
        switch (item->checkState(column))
        {
        case Qt::PartiallyChecked:
            item->setCheckState(column, Qt::Unchecked);
            break;
        case Qt::Unchecked:
            item->setCheckState(column, Qt::Checked);
            break;
        default:
            break;
        }
    }
    qDebug() << item->text(column) << "column" << column;
    updateSelectFileTip();
}

void TestSuite::itemEntered(QTreeWidgetItem *item, int column)
{
    if (QApplication::mouseButtons() & Qt::LeftButton)
    {
        QPoint pos = QCursor::pos();
        QScrollBar *ScrollBar = ui->treeWidget_SearchResult->verticalScrollBar();
        QRect ScrollBarRect = ScrollBar->geometry();
        if (!ScrollBarRect.contains(ScrollBar->mapFromGlobal(pos)))
        {
            item->setCheckState(column, item->checkState(column) == Qt::Checked ? Qt::Unchecked : Qt::Checked);
        }
    }
    updateSelectFileTip();
}

void TestSuite::updateSelectFileTip()
{
    // 更新选中的信息到 label 上
    int selectCount = 0;
    for (int i = 0; i < ui->treeWidget_SearchResult->topLevelItemCount(); ++i)
    {
        QTreeWidgetItem *item = ui->treeWidget_SearchResult->topLevelItem(i);
        for (int j = 0; j < item->childCount(); ++j)
        {
            if (item->child(j)->checkState(0) == Qt::Checked)
            {
                ++selectCount;
            }
        }
    }
    QString selectCountStr = "当前共选中" + QString::number(selectCount) + "个脚本";
    ui->label_SelectTip->setText(selectCountStr);
}

void TestSuite::initCheckBox()
{
    ui->checkBox_TestSuite->setCheckState(Qt::Checked);
    ui->checkBox_Chkcfg->setToolTip("<span style='font-size:12pt;'>每个脚本执行完后检查设备配置（较为耗时）</span>");
    ui->checkBox_Optimized->setToolTip("<span style='font-size:12pt;'>topo优化执行</span>");
    ui->checkBox_Shutdown->setToolTip("<span style='font-size:12pt;'>每次测试集跑完时shutdown接口</span>");
    ui->checkBox_Separator->setToolTip("<span style='font-size:12pt;'>每隔一定数量位置添加一行分隔符</span>");
    ui->checkBox_TotalTestSuite->setToolTip("<span style='font-size:12pt;'>创建一个包含所有测试集的.tst文件，默认文件名为Main.tst</span>");
    ui->checkBox_TestSuite->setToolTip("<span style='font-size:12pt;'>创建指定路径下所有模块的测试集</span>");
}

void TestSuite::initLineEdit()
{
}

void TestSuite::initComboBox()
{
    ui->comboBox_FileFilter->addItems(QStringList() << "*.tcl"
                                                    << "*.topo"
                                                    << "*.xml"
                                                    << "*.tbd"
                                                    << "*.tst");
    for (int i = 0; i <= 20; ++i)
    {
        ui->comboBox_Separator->addItem(QString::number(i));
    }
    ui->comboBox_Separator->setCurrentText("5");
    ui->comboBox_Separator->setEnabled(false);
}

void TestSuite::initTimer()
{
    // 实例化定时器对象
    timer = new QTimer(this);
    timer->setInterval(800);
    connect(timer, &QTimer::timeout, this, [=]()
            {
        ui->checkBox_TotalTestSuite->setStyleSheet("background-color: #f0f0f0");
        ui->checkBox_TestSuite->setStyleSheet("background-color: #f0f0f0");
        ui->label_Tip->setText(""); });
}

void TestSuite::initTreeWidget()
{
    ui->treeWidget_SearchResult->setHeaderHidden(true);
    // 连接到文件查找函数
    connect(ui->pushButton_Search, &QPushButton::clicked, this, &TestSuite::SearchFiles_and_UpdateTreeWidget);
    connect(ui->treeWidget_SearchResult, &QTreeWidget::itemChanged, this, &TestSuite::itemChanged);
    connect(ui->treeWidget_SearchResult, &QTreeWidget::itemClicked, this, &TestSuite::itemClicked);
    connect(ui->treeWidget_SearchResult, &QTreeWidget::itemEntered, this, &TestSuite::itemEntered);
}

void TestSuite::initPushButton()
{
    ui->pushButton_SelectDir->setIcon(QIcon(":/icon/folder.ico"));
    connect(ui->pushButton_SelectDir, &QPushButton::clicked, this, [&]()
            {
        if  (historicalPath.isEmpty()){
            historicalPath = "./";
        }
        historicalPath = QFileDialog::getExistingDirectory(this, "选择项目文件夹", historicalPath);
        if(!historicalPath.isEmpty())
            {
            ui->lineEdit_Dir->setText(historicalPath);
        } });

    ui->pushButton_Create->setEnabled(false);
    connect(ui->pushButton_Create, &QPushButton::clicked, this, [&]()
            {
                if (ui->checkBox_TestSuite->checkState() == Qt::Checked && ui->checkBox_TotalTestSuite->checkState() == Qt::Checked)
                {
                    createTestSuite();
                    createTotalTestSuite();
                }
                else if (ui->checkBox_TestSuite->checkState() == Qt::Checked)
                {
                    createTestSuite();
                }
                else if (ui->checkBox_TotalTestSuite->checkState() == Qt::Checked)
                {
                    createTotalTestSuite();
                }
                else
                {
                    timer->start();
                    // 通过着色来提示用户未选用创建模式
                    ui->checkBox_TotalTestSuite->setStyleSheet("background-color: #B8B8B8");
                    ui->checkBox_TestSuite->setStyleSheet("background-color: #B8B8B8");
                    ui->label_Tip->setText("未勾选创建内容!");
                } });
}

// 创建包含所有模块的测试集，即总测试集
void TestSuite::createTotalTestSuite()
{
    QString Content;
    QString scriptCountNum;
    QString line;
    QString totalTestSuitePath;
    QStringList fileList;
    int allScriptCount = 0;
    int maxLenLine = 0;
    static QRegularExpression re_ScriptCount("总数(\\d+)个");

    if (!ui->lineEdit_Dir->text().isEmpty())
    {
        // 过滤控件路径
        QDir dir(ui->lineEdit_Dir->text(), "*.tst");

        dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);

        QDirIterator it(dir, QDirIterator::Subdirectories);
        GBKFileIO io;

        // 第一遍获取里面最长的名字，用于对齐排版
        while (it.hasNext())
        {
            it.next();
            if (it.fileName() != "Main.tst")
            {
                line = "<SOURCE> \"" + it.filePath() + "\"";
                if (line.length() > maxLenLine)
                {
                    maxLenLine = line.length();
                }
                fileList << it.filePath();
            }
        }

        // 遍历 fileList
        for (const QString &file : fileList)
        {
            line = "<SOURCE> \"" + file + "\"";
            scriptCountNum = re_ScriptCount.match(io.readFile(file)).captured(1);
            if (scriptCountNum.isEmpty())
            {
                allScriptCount += 0;
                line = line.leftJustified(maxLenLine + 10, ' ') + ";# 无脚本\n";
            }
            else
            {
                allScriptCount += scriptCountNum.toInt();
                line = line.leftJustified(maxLenLine + 10, ' ') + ";# 共" + scriptCountNum + "个脚本\n";
            }
            Content.append(line);
        }
        Content = "# 共" + QString::number(allScriptCount) + "个脚本\n" + Content;
        // 构建测试总集文件名，并创建写入
        totalTestSuitePath = ui->lineEdit_Dir->text() + "/Main.tst";
        io.writeFile(totalTestSuitePath, Content);
    }
    else
    {
        ui->label_Tip->setText("path is empty !");
    }
}

// 测试集创建
void TestSuite::createTestSuite()
{
    // 当 tree选中为空、或者压根没创建 tree时，就不需要往下执行了
    if (ui->treeWidget_SearchResult->topLevelItemCount() == 0)
    {
        return;
    }

    int scriptCount = 0;

    QString tstHeader = "<TESTSUITE_BEGIN>\n    <TESTSUITE> description \"File created by hkf9092\" shutdown_intf 0 optimized 1 chkcfg 0\n";
    QString tstTail = "<TESTSUITE_END>";

    QString fileContent;
    QString lineStr;
    QString lineStrAll;
    QString scriptCountTips;
    QString tstContent;
    QString tstFilePath;
    QString finishTip;
    GBKFileIO io;

    static QRegularExpression re_TITLE("<TITLE>\\s+\"(.*)\"");
    static QRegularExpression re_AUTHOR("<AUTHOR>\\s+\"(.*)\"");

    short int defaultLen = 20;
    short int maxNameLen = 0;
    short int totalStrLen = 0;
    short int totalStrLen2 = 0;
    short int maxTitleLen = 0;

    // 获取 tree 控件上所有 Checked 状态的子控件信息
    QMap<QString, QStringList> mapDirFile;
    for (int i = 0; i < ui->treeWidget_SearchResult->topLevelItemCount(); ++i)
    {
        QTreeWidgetItem *item = ui->treeWidget_SearchResult->topLevelItem(i);
        for (int j = 0; j < item->childCount(); ++j)
        {
            if (item->child(j)->checkState(0) == Qt::Checked)
            {
                mapDirFile[item->text(0)] << item->text(0).append("/" + item->child(j)->text(0));
            }
        }
    }

    // 2层嵌套循环遍历，构造 tst 文件内容
    // it.key() : 目录
    // it.value() : tcl文件
    for (QMap<QString, QStringList>::const_iterator it = mapDirFile.constBegin(); it != mapDirFile.constEnd(); ++it)
    {
        // 按顺序读取目录路径，以构 tst 文件路径
        QFileInfo dirInfo(it.key());
        tstFilePath = dirInfo.absoluteFilePath() + "/" + dirInfo.fileName() + ".tst";
        for (const QString &file : it.value())
        {
            QFileInfo fileInfo(file);

            // 读取文件
            fileContent = io.readFile(file);

            // 读取 标题和作
            const QString &title = re_TITLE.match(fileContent).captured(1);

            if (fileInfo.fileName().length() > maxNameLen)
            {
                maxNameLen = fileInfo.fileName().length();
            }
            if (title.length() > maxTitleLen)
            {
                maxTitleLen = title.length();
            }
        }

        // 设置字符串总长度为:  8个空 + "<INCLUDE> \"" + 最长文件名 + "\"" + 预留 10 个空格做间隔
        totalStrLen = defaultLen + maxNameLen + 10;
        totalStrLen2 = totalStrLen + maxTitleLen + 13;

        for (const QString &file : it.value())
        {
            ++scriptCount;

            // 读取文件
            fileContent = io.readFile(file);

            // 读取 标题和作
            const QString &title = re_TITLE.match(fileContent).captured(1);
            const QString &author = re_AUTHOR.match(fileContent).captured(1);
            QFileInfo fileInfo(file);

            // 格式化对 各文本内
            lineStr = "        <INCLUDE> \"" + fileInfo.fileName() + "\"";
            lineStr = lineStr.leftJustified(totalStrLen, ' ') + ";# " + title;
            lineStr = lineStr.leftJustified(totalStrLen2, ' ') + "作:" + author;
            lineStrAll = lineStrAll + lineStr + "\n";
        }

        scriptCountTips = "# 脚本总数" + QString::number(scriptCount) + "个\n";
        tstContent = scriptCountTips + tstHeader + lineStrAll + tstTail;

        // 覆盖写入创建 tst 文件
        io.writeFile(tstFilePath, tstContent);

        finishTip = tstFilePath + "创建完成";
        ui->label_Tip->setText(finishTip);

        // 变量重置
        lineStrAll = "";
        maxNameLen = 0;
        maxTitleLen = 0;
        scriptCount = 0;
    }

    ui->label_Tip->setText("所有模块创建完毕");
}

// 文件查找与更新
void TestSuite::SearchFiles_and_UpdateTreeWidget()
{
    QIcon ico_folder(":/ico/icon/folder.ico");
    QIcon ico_file(":/ico/icon/file.ico");
    QMap<QString, QStringList> mapDirFiles;
    GBKFileIO io;

    ui->treeWidget_SearchResult->clear();
    // 滤掉控件上指定的文件类型
    QDir dir(ui->lineEdit_Dir->text(), ui->comboBox_FileFilter->currentText());

    // 滤掉 "." ".." 文件夹类型，但是遍历文件夹与文件
    dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

    QDirIterator it(dir, QDirIterator::Subdirectories);

    // 迭代遍历所有目录下的文件、以及子目录下的文件（迭代器默认只遍历文件）
    while (it.hasNext())
    {
        it.next();
        // 往容器里添 目录  目录中所有的文件
        mapDirFiles[it.fileInfo().dir().absolutePath()] << it.filePath();
    }

    // 遍历容器内容，并插入到对应目录树
    QMap<QString, QStringList>::iterator i = mapDirFiles.begin();
    while (i != mapDirFiles.end())
    {
        QTreeWidgetItem *topItem = new QTreeWidgetItem(ui->treeWidget_SearchResult);
        topItem->setIcon(0, ico_folder);
        topItem->setText(0, i.key());

        for (const QString &file : i.value())
        {
            QTreeWidgetItem *item = new QTreeWidgetItem(topItem);
            QFileInfo fileInfo(file);
            item->setText(0, fileInfo.fileName());
            item->setIcon(0, ico_file);
            if (
                !fileInfo.fileName().contains("setup", Qt::CaseInsensitive) &&
                !fileInfo.fileName().contains("clear", Qt::CaseInsensitive) &&
                !fileInfo.fileName().contains("PATCH", Qt::CaseInsensitive) &&
                !fileInfo.fileName().contains("JOJO", Qt::CaseInsensitive) &&
                !fileInfo.fileName().contains("DIO", Qt::CaseInsensitive) &&
                !fileInfo.fileName().contains("CustomFunction", Qt::CaseInsensitive))
            {
                item->setCheckState(0, Qt::Checked);
            }
            else
            {
                item->setCheckState(0, Qt::Unchecked);
            }
            item->setToolTip(0, io.readFile(file).mid(0,1000));
        }
        ++i;
    }
    ui->treeWidget_SearchResult->expandAll();
    ui->pushButton_Create->setEnabled(true);
}
