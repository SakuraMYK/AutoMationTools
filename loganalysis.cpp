#include "loganalysis.h"
#include "ui_loganalysis.h"

LogAnalysis::LogAnalysis(QWidget *parent) : QWidget(parent), ui(new Ui::LogAnalysis)
{
    isExpandAll = false;
    ui->setupUi(this);
    ui->pushButton_SelectDir->setIcon(QIcon(":/ico/folder.ico"));
    ui->treeWidget_SearchResult->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget_SearchResult, &QTreeWidget::customContextMenuRequested, this, &LogAnalysis::addOtherLogfileContextMenuActionsToItem);
    connect(ui->pushButton_Statistics, &QPushButton::clicked, this, &LogAnalysis::updateTreeWidget);
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

    connect(ui->pushButton_ExpandAll, &QPushButton::clicked, this, [&]()
            {
                if (!isExpandAll)
                {
                    ui->treeWidget_SearchResult->expandAll();
                    ui->pushButton_ExpandAll->setText("折叠");
                    isExpandAll = true;
                }
                else
                {
                    ui->treeWidget_SearchResult->collapseAll();
                    ui->pushButton_ExpandAll->setText("展开");
                    isExpandAll = false;
                } });

    // 为首行的每一格文本居中对齐
    for (int i = 0; i < ui->treeWidget_SearchResult->columnCount(); ++i)
    {
        ui->treeWidget_SearchResult->headerItem()->setTextAlignment(i, Qt::AlignCenter);
    }

    // 设置第一列（index为0）的宽度为自适应文本宽度
    ui->treeWidget_SearchResult->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);

    ui->lineEdit_Dir->setText("D:\\测试转发组脚本集");

    //    QString file = "D:\\测试转发组脚本集\\log\\GRE\\TestMaster_TS_BAS_GRE_10.1.11.41.2.13_1_1_4_20230426092744.xml";
    //    getTestSuiteLogContent(file);
}

LogAnalysis::~LogAnalysis()
{
    delete ui;
}

void LogAnalysis::addOtherLogfileContextMenuActionsToItem(const QPoint &pos)
{

    // 获取被点击的 QTreeWidgetItem
    QTreeWidgetItem *item = ui->treeWidget_SearchResult->itemAt(pos);

    if (item != nullptr && allLogInfo.contains(item->text(0)))
    {
        QMenu *menu = new QMenu(ui->treeWidget_SearchResult);
        QMenu *logs = menu->addMenu("切换其他时间段的log");
        QAction *openTmLog = new QAction("打开对应的TestMaster log", logs);
        connect(openTmLog, &QAction::triggered, this, [=]()
                { onAddItemTriggered(openTmLog); });

        for (const QString &startTime : allLogInfo[item->text(0)]["startTime"].toStringList())
        {
            QAction *addAction = new QAction(startTime, logs);

            // 将 item 作为上下文数据传递给 QAction 对象
            addAction->setData(QVariant::fromValue(item));

            // 连接信号和槽函数，并将 QAction 对象作为参数传递
            connect(addAction, &QAction::triggered, this, [=]()
                    { onAddItemTriggered(addAction); });

            logs->addAction(addAction);
        }
        menu->addAction(openTmLog);
        menu->exec(ui->treeWidget_SearchResult->mapToGlobal(pos));
    }
    else
    {
        qDebug() << "当前右键选中的不是 allLogInfo 中存在的键!";
    }
}

void LogAnalysis::onAddItemTriggered(QAction *action)
{

    // 获取 QAction 对象中保存的上下文数据
    //    QTreeWidgetItem* item = action->data().value<QTreeWidgetItem*>();

    qDebug() << "当前选中：" + action->text();

    // 同时更新子控件下对应的所有文件信息
    // ...
}

QString LogAnalysis::getTestSuiteName(QStringList &fileNameList)
{

    QString str = fileNameList[0];
    for (int i = 1; i < fileNameList.count(); ++i)
    {
        str = LongestCommonSubstring(str, fileNameList[i]);
    }

    // 如果以下划线结尾，就去掉最后的一个下划线
    if (str.endsWith('_'))
    {
        str = str.mid(0, str.length() - 1);
    }

    return str;
}

// 获取两个字符串的最长公共连续子串（动态规划）
QString LogAnalysis::LongestCommonSubstring(QString &a, QString &b)
{

    int len_a = a.length() + 1;
    int len_b = b.length() + 1;

    // 容器创建一个二维矩阵，并全部初始为0
    std::vector<std::vector<int>> dp(len_a, std::vector<int>(len_b, 0));

    int max = 0;
    int start = 0;

    // dp的下标要从1开始，以忽略首行、首列的0
    for (int i = 1; i < len_a; ++i)
    {
        for (int j = 1; j < len_b; ++j)
        {
            // 字符串取值的下标要 -1 ，因为包含了首行、首列
            if (a[i - 1] == b[j - 1])
            {
                dp[i][j] = dp[i - 1][j - 1] + 1;
                if (dp[i - 1][j - 1] + 1 > max)
                {
                    max = dp[i - 1][j - 1] + 1;
                    start = i - max;
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

    return a.mid(start, max);
}

// 从指定目录中获取所有测试集的log以及TestMaster的log
QMap<QString, QMap<QString, QVariant>> LogAnalysis::createAllLogInfoDictionary()
{

    QMap<QString, QStringList> mapDirFiles;

    QDir dir(ui->lineEdit_Dir->text(), "*.xml");
    qDebug() << "select dir:" << dir;
    QDirIterator it(dir, QDirIterator::Subdirectories);

    // 迭代遍历所有目录下的文件、以及子目录下的文件（迭代器默认只遍历文件）
    while (it.hasNext())
    {
        it.next();
        // 只能传入文件名，不能传入绝对路径或相对路径，否则可能会计算出比tst名更长的子串
        mapDirFiles[it.fileInfo().dir().absolutePath()] << it.fileName();
    }

    if (mapDirFiles.isEmpty())
    {
        qDebug() << "No log file found!";
        return allLogInfo;
    }

    QString tstName;
    QStringList logList;
    QStringList tmLogList;
    QStringList startTimeList;

    const QStringList &mapKeys = mapDirFiles.keys();
    for (const QString &dir : mapKeys)
    {

        tstName = getTestSuiteName(mapDirFiles[dir]);
        allLogInfo[tstName]["dir"] = dir;

        QRegularExpression re_logName("^" + tstName + "_(\\d{14})\\.xml");
        QRegularExpression re_TMLogName("^TestMaster_" + tstName + "_(\\d{14})\\.xml");

        for (QString &file : mapDirFiles[dir])
        {
            if (re_TMLogName.match(file).hasMatch())
            {
                tmLogList << dir + "/" + file;
                startTimeList << QDateTime::fromString(file.mid(file.lastIndexOf('_') + 1, 14), "yyyyMMddhhmmss").toString("yyyy/MM/dd hh:mm:ss");
            }
            else if (re_logName.match(file).hasMatch())
            {
                logList << dir + "/" + file;
                startTimeList << QDateTime::fromString(file.mid(file.lastIndexOf('_') + 1, 14), "yyyyMMddhhmmss").toString("yyyy/MM/dd hh:mm:ss");
            }
        }

        allLogInfo[tstName]["logList"] = logList;
        allLogInfo[tstName]["tmLogList"] = tmLogList;
        allLogInfo[tstName]["startTime"] = startTimeList;

        logList.clear();
        tmLogList.clear();
        startTimeList.clear();
    }

    return allLogInfo;
}

// 抓取测试套的统计信息
QMap<QString, QString> LogAnalysis::getTestSuiteLogContent(const QString &filePath)
{
    QMap<QString, QString> map;
    QFile file(filePath);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug() << "failed to open" << filePath;

        return map;
    }
    QString xmlContent = file.readAll();
    static QRegularExpression re_OK_Num("<SECTION>\\s*<TITLE>OK<\\/TITLE>\\s*<VALUE><!\\[CDATA\\[(\\d+)\\]\\]><\\/VALUE>\\s*<\\/SECTION>");
    static QRegularExpression re_NG_Num("<SECTION>\\s*<TITLE>NG<\\/TITLE>\\s*<VALUE><!\\[CDATA\\[(\\d+)\\]\\]><\\/VALUE>\\s*<\\/SECTION>");
    static QRegularExpression re_ER_Num("<SECTION>\\s*<TITLE>ER<\\/TITLE>\\s*<VALUE><!\\[CDATA\\[(\\d+)\\]\\]><\\/VALUE>\\s*<\\/SECTION>");
    static QRegularExpression re_InvalidHead_Num("<SECTION>\\s*<TITLE>Invalid Head<\\/TITLE>\\s*<VALUE><!\\[CDATA\\[(\\d+)\\]\\]><\\/VALUE>\\s*<\\/SECTION>");
    static QRegularExpression re_UseTime("<CONTENT type = \"text\"><!\\[CDATA\\[(.*?)\\]\\]></CONTENT>");
    if (re_OK_Num.match(xmlContent).hasMatch())
    {
        map["OK"] = re_OK_Num.match(xmlContent).captured(1);
    }
    if (re_NG_Num.match(xmlContent).hasMatch())
    {
        map["NG"] = re_NG_Num.match(xmlContent).captured(1);
    }
    if (re_ER_Num.match(xmlContent).hasMatch())
    {
        map["ER"] = re_ER_Num.match(xmlContent).captured(1);
    }
    if (re_InvalidHead_Num.match(xmlContent).hasMatch())
    {
        map["Invalid Head"] = re_InvalidHead_Num.match(xmlContent).captured(1);
    }
    if (re_UseTime.match(xmlContent).hasMatch())
    {
        map["Use Time"] = re_UseTime.match(xmlContent).captured(1);
    }
    qDebug() << map;
    return map;
}

// 抓取测试脚本的细节信息
QMap<QString, QString> LogAnalysis::getScriptLogContent(const QString &filePath)
{

    QMap<QString, QString> map;
    QFile file(filePath);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug() << "failed to open" << filePath;
    }
    QString xmlContent = file.readAll();
    static QRegularExpression re_isTestCaseEnd("<CONTENT type = \"text\"><!\\[CDATA\\[TestCase End\\]\\]></CONTENT>");

    if (re_isTestCaseEnd.match(xmlContent).hasMatch())
    {
        map["isTestCaseEnd"] = "true";
    }
    else
    {
        map["isTestCaseEnd"] = "false";
    }
    qDebug() << map;

    return map;
}

QStringList LogAnalysis::getAllTclScriptsFromLog(const QString &logFilePath)
{
    QStringList tclScripts;
    static QRegularExpression re_ExecuteTclFile("<a href=.*>(.*?)</a>");
    // 打开日志文件并逐行读取
    QFile file(logFilePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        while (!in.atEnd())
        {
            QRegularExpressionMatch re = re_ExecuteTclFile.match(in.readLine());
            if (re.hasMatch())
            {
                tclScripts << re.captured(1);
            }
        }
        file.close();
    }
    else
    {
        qDebug() << logFilePath << "does not exist or cannot be opened";
    }
    return tclScripts;
}

void LogAnalysis::updateTreeWidget()
{
    qDebug() << "here0";

    ui->treeWidget_SearchResult->clear();
    createAllLogInfoDictionary();
    qDebug() << "here0.1";

    const QStringList &allLogInfoKeys = allLogInfo.keys();
    qDebug() << "here1";

    // 排序，以 startTime 为基准从日期的新到旧进行排序
    for (const QString &tst : allLogInfoKeys)
    {
        QStringList dirList;
        QStringList sortTimeList;
        QStringList logList;
        QStringList tmLogList;

        QMap<QString, QVariant> &logData = allLogInfo[tst];
        // 先对当前这个 tst 里面的所有 startTime 进行排序
        sortTimeList = allLogInfo[tst]["startTime"].toStringList();
        std::sort(sortTimeList.begin(), sortTimeList.end(), [](const QString &t1, const QString &t2)
                  { return t1 > t2; });

        logData["startTime"] = sortTimeList;
        for (const QString &newTime : sortTimeList)
        {
            int index = allLogInfo[tst]["startTime"].toStringList().indexOf(newTime);

            // 确保数组不会越界导致程序崩溃
            if (index >= 0 && index < allLogInfo[tst]["dir"].toStringList().length())
            {
                dirList.append(allLogInfo[tst]["dir"].toStringList()[index]);
            }
            if (index >= 0 && index < allLogInfo[tst]["logList"].toStringList().length())
            {
                logList.append(allLogInfo[tst]["logList"].toStringList()[index]);
            }
            if (index >= 0 && index < allLogInfo[tst]["tmLogList"].toStringList().length())
            {
                tmLogList.append(allLogInfo[tst]["tmLogList"].toStringList()[index]);
            }
        }

        allLogInfo[tst]["dir"] = dirList;
        allLogInfo[tst]["logList"] = logList;
        allLogInfo[tst]["tmLogList"] = tmLogList;

        dirList.clear();
        logList.clear();
        tmLogList.clear();
    }
    //    return;
    qDebug() << "here2";
    // 将遍历信息更新到控件上
    for (const QString &tst : allLogInfoKeys)
    {
        curTime = QDateTime::currentDateTime().toString(Qt::ISODate) + ":";

        QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget_SearchResult);
        item->setText(0, tst);
        item->setIcon(0, QIcon(":/ico/tst.ico"));

        qDebug() << curTime << "========================================";
        qDebug() << curTime << "tstName:        " << tst;
        qDebug() << curTime << "dir:            " << allLogInfo[tst]["dir"].toString();
        qDebug() << curTime << "startTime:      " << allLogInfo[tst]["startTime"].toStringList();
        qDebug() << curTime << "logList:        " << allLogInfo[tst]["logList"].toStringList();
        qDebug() << curTime << "tmLogList:      " << allLogInfo[tst]["tmLogList"].toStringList();

        for (const QString &logFile : allLogInfo[tst]["logList"].toStringList())
        {
            QStringList allTclScripts = getAllTclScriptsFromLog(logFile);
            for (const QString &tclFile : allTclScripts)
            {
                QTreeWidgetItem *tcl = new QTreeWidgetItem(item);
                tcl->setText(0, tclFile);
                tcl->setIcon(0, QIcon(":/ico/file_tcl.ico"));
            }
        }
    }
}
