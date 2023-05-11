#include "loganalysis.h"
#include "ui_loganalysis.h"

LogAnalysis::LogAnalysis(QWidget *parent) : QWidget(parent), ui(new Ui::LogAnalysis)
{
    ui->setupUi(this);
    ui->pushButton_SelectDir->setIcon(QIcon(":/ico/folder.ico"));
    ui->treeWidget_SearchResult->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget_SearchResult, &QTreeWidget::customContextMenuRequested, this, &LogAnalysis::addOtherLogfileContextMenuActionsToItem);

    connect(ui->pushButton_SelectDir, &QPushButton::clicked, this, [&]()
            {
        if  (historicalPath.isEmpty()){
            historicalPath = "./";
        }
        historicalPath = QFileDialog::getExistingDirectory(this, "选择log文件夹", historicalPath);
        if(!historicalPath.isEmpty())
            {
            ui->lineEdit_Dir->setText(historicalPath);
        } });

    // 为首行的每一格文本居中对齐
    for (int i = 0; i < ui->treeWidget_SearchResult->columnCount(); ++i) {
        ui->treeWidget_SearchResult->headerItem()->setTextAlignment(i,Qt::AlignCenter);
    }

    // 设置第一列（index为0）的宽度为自适应文本宽度
    ui->treeWidget_SearchResult->header()->setSectionResizeMode(0,QHeaderView::ResizeToContents);


    ui->lineEdit_Dir->setText("D:\\测试转发组脚本集\\log");
    getAllTestSuiteLog();
}

LogAnalysis::~LogAnalysis()
{
    delete ui;
}

void LogAnalysis::addOtherLogfileContextMenuActionsToItem(const QPoint& pos)
{
//    qDebug() << "into addOtherLogfileContextMenuActionsToItem";
    QMenu *menu = new QMenu(ui->treeWidget_SearchResult);
    QMenu *otherLogs = menu->addMenu("选则其他时间段的log");
    QMenu *openTMLog = menu->addMenu("打开对应的 TestMaster log");
    QAction* addAction1 = new QAction("Add Item1 迭代", ui->treeWidget_SearchResult);
    QAction* addAction2 = new QAction("Add Item2 丰富的", ui->treeWidget_SearchResult);
    QAction* addAction3 = new QAction("Add Item3 33阿", ui->treeWidget_SearchResult);
    QAction* addAction4 = new QAction("Add Item4 打发撒旦", ui->treeWidget_SearchResult);

    QAction* addAction_OpenTMLog = new QAction("TestMaster_TS_BAS_GRE_10.1.11.41_20230426082637.xml", ui->treeWidget_SearchResult);

    // 获取被点击的 QTreeWidgetItem
    QTreeWidgetItem* item = ui->treeWidget_SearchResult->itemAt(pos);

    // 将 item 作为上下文数据传递给 QAction 对象
    addAction1->setData(QVariant::fromValue(item));
    addAction2->setData(QVariant::fromValue(item));
    addAction3->setData(QVariant::fromValue(item));
    addAction4->setData(QVariant::fromValue(item));

    // 连接信号和槽函数，并将 QAction 对象作为参数传递
    connect(addAction1, &QAction::triggered, this, [=]() { onAddItemTriggered(addAction1); });
    connect(addAction2, &QAction::triggered, this, [=]() { onAddItemTriggered(addAction2); });
    connect(addAction3, &QAction::triggered, this, [=]() { onAddItemTriggered(addAction3); });
    connect(addAction4, &QAction::triggered, this, [=]() { onAddItemTriggered(addAction4); });

    otherLogs->addAction(addAction1);
    otherLogs->addAction(addAction2);
    otherLogs->addAction(addAction3);
    otherLogs->addAction(addAction4);

    openTMLog->addAction(addAction_OpenTMLog);
    menu->exec(ui->treeWidget_SearchResult->mapToGlobal(pos));
    delete menu;
    delete addAction1;
    delete addAction2;
    delete addAction3;
    delete addAction4;
}

void LogAnalysis::onAddItemTriggered(QAction* action)
{
    // 获取 QAction 对象中保存的上下文数据
    QTreeWidgetItem* item = action->data().value<QTreeWidgetItem*>();
    item->setText(0,action->text());

    // 同时更新子控件下对应的所有文件信息
    // ...
}

QString LogAnalysis::getTestSuiteName(QStringList& fileNameList) {

    QString str = fileNameList[0];
    for (int i = 1; i < fileNameList.count(); ++i) {
        str = LongestCommonSubstring(str,fileNameList[i]);
    }

    // 如果以下划线结尾，那么就删掉最后的一个下划线
    if(str.endsWith('_')){
        str = str.mid(0,str.length()-1);
    }
    return str;
}

// 获取两个字符串的最长公共连续子串（动态规划）
QString LogAnalysis::LongestCommonSubstring(QString &a,QString &b) {

    int len_a = a.length() + 1;
    int len_b = b.length() + 1;

    // 容器创建一个二维矩阵，并全部初始为0
    std::vector<std::vector<int>>dp(len_a,std::vector<int>(len_b,0));

    int max = 0;
    int start = 0;

    // dp的下标要从1开始，以忽略首行、首列的0
    for (int i = 1; i < len_a; ++i) {
        for (int j = 1; j < len_b; ++j) {
            // 字符串取值的下标要 -1 ，因为包含了首行、首列
            if(a[i-1] == b[j-1])
            {
                dp[i][j]=dp[i-1][j-1]+1;
                if(dp[i-1][j-1]+1 > max)
                {
                    max = dp[i-1][j-1]+1;
                    start = i-max;
                }
            }
        }
    }


//    // debug 打印操作
//    QString str;
//    QString str2 = "  ";
//    for (int i = 1; i < len_b; ++i) {
//        str2.append(b[i-1]).append(" ");
//    }
//    qDebug() << str2;
//    for (int i = 1; i < len_a; ++i) {
//        str.append(a[i-1]).append(" ");
//        for (int j = 1; j < len_b; ++j) {
//            str.append(QString::number(dp[i][j]).append(" "));
//        }
//        qDebug() << str;
//        str = "";
//    }

//    qDebug() <<"最长子串为：" <<a.mid(start,max);
    return a.mid(start,max);
}

// 从所有文件中获取测试集的log以及TestMaster的log
QVector<LogAnalysis::dirLogs> LogAnalysis::getAllTestSuiteLog()
{
    QMap<QString, QStringList> mapDirFiles;

    dirLogs dirlog;
    QVector<dirLogs> allXmlFiles;

    QDir dir(ui->lineEdit_Dir->text(),"*.xml");
    QDirIterator it(dir,QDirIterator::Subdirectories);

    // 迭代遍历所有目录下的文件、以及子目录下的文件（迭代器默认只遍历文件）
    while (it.hasNext())
    {
        it.next();
        // 只能传入文件名，不能传入绝对路径或相对路径，否则会计算出绝对路径里比tst名更长的子串
        mapDirFiles[it.fileInfo().dir().absolutePath()] << it.fileName();
    }

    for (QMap<QString, QStringList>::Iterator i = mapDirFiles.begin(); i != mapDirFiles.end(); ++i) {
        QRegularExpression re_logName("^" + getTestSuiteName(i.value()) + "_(\\d{14})\\.xml");
        QRegularExpression re_TMLogName("^TestMaster_" + getTestSuiteName(i.value()) + "_(\\d{14})\\.xml");
        dirlog.dir = i.key();
        for (QString & file : i.value()) {
            if(re_TMLogName.match(file).hasMatch())
            {
                dirlog.logList << i.key() + "/" + file;
            }
            else if(re_logName.match(file).hasMatch())
            {
                dirlog.tmLogList << i.key() + "/" + file;
            }
        }
        allXmlFiles.append(dirlog);
        dirlog.logList.clear();
        dirlog.tmLogList.clear();
    }

//    for (const auto & tt: allXmlFiles)
//    {
//        qDebug() << "==============";
//        qDebug() << "目录：      " << tt.dir;
//        qDebug() << "log文件：   " << tt.logList;
//        qDebug() << "TM log文件：" << tt.tmLogList;
//    }
    return allXmlFiles;
}
