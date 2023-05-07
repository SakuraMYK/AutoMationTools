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


    getAllTestSuiteLog();
}

LogAnalysis::~LogAnalysis()
{
    delete ui;
}

void LogAnalysis::addOtherLogfileContextMenuActionsToItem(const QPoint& pos)
{
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

QString LogAnalysis::getTestSuiteName(QStringList& fileList) {

    QString str = fileList[0];
    for (int i = 1; i < fileList.count(); ++i) {
        str = LongestCommonSubstring(str,fileList[i]);
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


QStringList LogAnalysis::getAllTestSuiteLog()
{
    QStringList fileList;

    fileList
        << "TestMaster_TS_BAS_GRE_10.1.11.41.2.108.5_1_1_3_20230426185211.xml"
        << "TS_BAS_GRE_10.1.11.41.2.108.5_1_1_3_20230426185211.xml"
        << "TS_BAS_GRE_10.1.11.41.2.108.5_1_1_3_20230426184812.xml"
        << "TestMaster_TS_BAS_GRE_10.1.11.41.2.108.5_1_1_3_20230426184258.xml"
        << "TS_BAS_GRE_10.1.11.41.2.108.5_1_1_3_20230426184258.xml"
        << "TS_BAS_GRE_10.1.11.41_setup_3_20230426184339.xml"
        << "TS_BAS_GRE_10.1.11.41_20290226881234.xml"
        << "TestMaster_TS_BAS_GRE_10.1.11.41_setup_3_20230426184339.xml"
        << "TestMaster_TS_BAS_GRE_10.1.11.41.2.108.5_1_1_3_20230426165537.xml"
        << "TS_BAS_GRE_10.1.11.41.2.108.5_1_1_3_20230426165537.xml"
        << "TS_BAS_GRE_10.1.11.41_setup_3_20230426165617.xml"
        << "TestMaster_TS_BAS_GRE_10.1.11.41_setup_3_20230426165617.xml"
        << "TestMaster_TS_BAS_GRE_10.1.11.41.2.108.5_1_1_3_20230426163905.xml"
        << "TS_BAS_GRE_10.1.11.41.2.108.5_1_1_3_20230426163905.xml"
        << "TS_BAS_GRE_10.1.11.41_clear_3_20230426164527.xml"
        << "TestMaster_TS_BAS_GRE_10.1.11.41_clear_3_20230426164527.xml"
        << "TS_BAS_GRE_10.1.11.41_setup_3_20230426163945.xml"
        << "TestMaster_TS_BAS_GRE_10.1.11.41_setup_3_20230426163945.xml"
        << "TS_BAS_GRE_10.1.11.41_setup_3_20230426141253.xml"
        << "TestMaster_TS_BAS_GRE_10.1.11.41_setup_3_20230426141253.xml"
        << "TestMaster_TS_BAS_GRE_10.1.11.41.2.108.8_1_1_4_20230426135050.xml"
        << "TS_BAS_GRE_10.1.11.41.2.108.8_1_1_4_20230426135050.xml"
        << "TS_BAS_GRE_10.1.11.41_clear_4_20230426135817.xml"
        << "TestMaster_TS_BAS_GRE_10.1.11.41_clear_4_20230426135817.xml"
        << "TS_BAS_GRE_10.1.11.41_setup_4_20230426135149.xml"
        << "TestMaster_TS_BAS_GRE_10.1.11.41_setup_4_20230426135149.xml"
        << "TestMaster_TS_BAS_GRE_10.1.11.41_20230426082637.xml"
        << "TS_BAS_GRE_10.1.11.41_20230426082637.xml"
        << "TS_BAS_GRE_10.1.11.41_clear_4_20230426092744.xml"
        << "TS_BAS_GRE_10.1.11.41.2.108.8_1_1_4_20230426093155.xml"
        << "TestMaster_TS_BAS_GRE_10.1.11.41.2.108.8_1_1_4_20230426093155.xml"
        << "TS_BAS_GRE_10.1.11.41.2.13_1_1_4_20230426092744.xml"
        << "TestMaster_TS_BAS_GRE_10.1.11.41.2.13_1_1_4_20230426092744.xml"
        << "TestMaster_TS_BAS_GRE_10.1.11.41_clear_4_20230426092744.xml"
        << "TS_BAS_GRE_10.1.11.41_setup_4_20230426092439.xml"
        << "TestMaster_TS_BAS_GRE_10.1.11.41_20230426081234.xml"
        << "TS_BAS_GRE_10.1.11.41_20230426886521.xml"
        << "TestMaster_TS_BAS_GRE_10.1.11.41_setup_4_20230426092439.xml"
        << "TS_BAS_GRE_10.1.11.41_clear_3_20230426092206.xml"
        << "TS_BAS_GRE_10.1.11.41.2.108.7_1_1_3_20230426092206.xml";

//    qDebug() << getTestSuiteName(fileList);
    QRegularExpression re_TstName("^" + getTestSuiteName(fileList) + "_(\\d{14})\\.xml");
    for(QString & file:fileList)
    {
        if(re_TstName.match(file).hasMatch())
        {
            qDebug() <<file;
        }
    }
    return QStringList();
}
