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
    getAllLogFiles();
//    updateTreeWidget();
}

LogAnalysis::~LogAnalysis()
{
    delete ui;
}

void LogAnalysis::addOtherLogfileContextMenuActionsToItem(const QPoint& pos)
{
    // 获取被点击的 QTreeWidgetItem
    QTreeWidgetItem* item = ui->treeWidget_SearchResult->itemAt(pos);

    QMenu *menu = new QMenu(ui->treeWidget_SearchResult);

    QMenu *otherLogs = menu->addMenu("选则其他时间段的log");
    QMenu *openTMLog = menu->addMenu("对应的TestMaster log");

    QAction* addAction1 = new QAction("log1", ui->treeWidget_SearchResult);
    QAction* addAction2 = new QAction("log2", ui->treeWidget_SearchResult);
    QAction* addAction3 = new QAction("log3", ui->treeWidget_SearchResult);
    QAction* addAction4 = new QAction("log4", ui->treeWidget_SearchResult);

    QAction* addAction_OpenTMLog = new QAction("TM log", ui->treeWidget_SearchResult);


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

}

void LogAnalysis::onAddItemTriggered(QAction* action)
{
    // 获取 QAction 对象中保存的上下文数据
    QTreeWidgetItem* item = action->data().value<QTreeWidgetItem*>();
    item->setText(0,action->text());
    qDebug() << "当前选中的是：" << item;
    // 同时更新子控件下对应的所有文件信息
    // ...
}

QString LogAnalysis::getTestSuiteName(QStringList& fileNameList) {

    QString str = fileNameList[0];
    for (int i = 1; i < fileNameList.count(); ++i) {
        str = LongestCommonSubstring(str,fileNameList[i]);
    }

    // 如果以下划线结尾，就去掉最后的一个下划线
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
    std::vector<std::vector<int>> dp(len_a,std::vector<int>(len_b,0));

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

// 从指定目录中获取所有测试集的log以及TestMaster的log
QMap<QString,QMap<QString,QVariant>> LogAnalysis::getAllLogFiles()
{
    // 定义一个数据结构，存放每个xml对应的各项内容
    //    模块名 {
    //        目录： XXX
    //        开始时间： XXX
    //        log文件列表： XXX XXX XXX
    //        TM log文件列表： XXX XXX XXX XXX
    //    }
    QMap<QString,QMap<QString,QVariant>> allLogInfo;

    QMap<QString, QStringList> mapDirFiles;

    QDir dir(ui->lineEdit_Dir->text(),"*.xml");
    QDirIterator it(dir,QDirIterator::Subdirectories);

    // 迭代遍历所有目录下的文件、以及子目录下的文件（迭代器默认只遍历文件）
    while (it.hasNext())
    {
        it.next();
        // 只能传入文件名，不能传入绝对路径或相对路径，否则可能会计算出比tst名更长的子串
        mapDirFiles[it.fileInfo().dir().absolutePath()] << it.fileName();
    }

    QString tstName;
    QStringList logList;
    QStringList tmLogList;
    QStringList startTimeList;

    const QStringList & mapKeys = mapDirFiles.keys();
    for (const QString & dir:mapKeys) {

        tstName = getTestSuiteName(mapDirFiles[dir]);
        allLogInfo[tstName]["dir"] = dir;

        QRegularExpression re_logName("^" + tstName + "_(\\d{14})\\.xml");
        QRegularExpression re_TMLogName("^TestMaster_" + tstName + "_(\\d{14})\\.xml");

        for (QString & file : mapDirFiles[dir]) {
            if(re_TMLogName.match(file).hasMatch())
            {
                logList << dir + "/" + file;
                startTimeList << QDateTime::fromString(file.mid(file.lastIndexOf('_') + 1,14), "yyyyMMddhhmmss").toString("yyyy/MM/dd hh:mm:ss");
            }
            else if(re_logName.match(file).hasMatch())
            {
                tmLogList << dir + "/" + file;
                startTimeList << QDateTime::fromString(file.mid(file.lastIndexOf('_') + 1,14), "yyyyMMddhhmmss").toString("yyyy/MM/dd hh:mm:ss");
            }
        }

        allLogInfo[tstName]["logList"] = logList;
        allLogInfo[tstName]["tmLogList"] = tmLogList;
        allLogInfo[tstName]["startTime"] = startTimeList;

        logList.clear();
        tmLogList.clear();
        startTimeList.clear();
    }

    // 信息打印
    const QStringList & allLogInfoKeys = allLogInfo.keys();
    for (const QString & key: allLogInfoKeys)
    {
        qDebug() <<"========================================";
        qDebug() << "模块：     " << key;
        qDebug() << "目录：     "<<allLogInfo[key]["dir"].toString();
        qDebug() << "开始时间：  "<<allLogInfo[key]["startTime"].toStringList();
        qDebug() << "logList：  "<<allLogInfo[key]["logList"].toStringList();
        qDebug() << "tmLogList："<<allLogInfo[key]["tmLogList"].toStringList();
    }

    return allLogInfo;
}

void LogAnalysis::updateTreeWidget()
{
//    for (auto&log:getAllLogFiles())
//    {
//        for (QString & file:log.logList)
//        {
//            QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget_SearchResult);
//            QFileInfo fileInfo(file);
//            item->setText(0,fileInfo.fileName().mid(0,fileInfo.fileName().length() - 19));
//            break;
//        }
//    }
}
