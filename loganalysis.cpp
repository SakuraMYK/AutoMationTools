#include "loganalysis.h"
#include "ui_loganalysis.h"

LogAnalysis::LogAnalysis(QWidget *parent) : QWidget(parent), ui(new Ui::LogAnalysis)
{
    ui->setupUi(this);
    isExpandAll = false;
    ui->pushButton_ExpandAll->setIconSize(QSize(32, 32));
    ui->pushButton_SelectDir->setIcon(QIcon(":/ico/folder.ico"));
    //    ui->pushButton_SelectDir->setIconSize(QSize(25,25));
    //    ui->pushButton_SelectDir->setStyleSheet("border-width: 0px;");
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
                    ui->pushButton_ExpandAll->setIcon(QIcon(":/ico/Collapse.ico"));
                    isExpandAll = true;
                }
                else
                {
                    ui->treeWidget_SearchResult->collapseAll();
                    ui->pushButton_ExpandAll->setText("展开");
                    ui->pushButton_ExpandAll->setIcon(QIcon(":/ico/Expand.ico"));
                    isExpandAll = false;
                } });

    // 为首行的每一格文本居中对齐
    for (int i = 0; i < ui->treeWidget_SearchResult->columnCount(); ++i)
    {
        ui->treeWidget_SearchResult->headerItem()->setTextAlignment(i, Qt::AlignCenter);
    }

    // 设置每列宽度为自适应文本宽度
    for (int i = 0; i < ui->treeWidget_SearchResult->header()->count(); ++i)
    {
        ui->treeWidget_SearchResult->header()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
    }

    ui->lineEdit_Dir->setText("D:\\log");

    //    QString file = "D:\\测试转发组脚本集\\log\\GRE\\TestMaster_TS_BAS_GRE_10.1.11.41.2.13_1_1_4_20230426092744.xml";
    //    getTestSuiteLogContent(file);
}

LogAnalysis::~LogAnalysis()
{
    delete ui;
}

#include <windows.h>

void LogAnalysis::addOtherLogfileContextMenuActionsToItem(const QPoint &pos)
{

    //    LPCWSTR url = reinterpret_cast<LPCWSTR>(ui->treeWidget_SearchResult->itemAt(pos)->toolTip(0).utf16());

    //    // 使用 ShellExecute 函数打开文件
    //    intptr_t result = reinterpret_cast<intptr_t>(ShellExecute(NULL, L"open", L"iexplore.exe", url, NULL, SW_SHOWNORMAL));

    //    if (result <= 32)
    //    {
    //        // 打开失败，可以根据具体错误进行处理
    //        // 使用 GetLastError() 获取错误代码
    //        qDebug() << ui->treeWidget_SearchResult->itemAt(pos)->toolTip(0) << "open failed!" << GetLastError();
    //    }

    //        QMenu *menu = new QMenu(ui->treeWidget_SearchResult);
    //        QMenu *logs = menu->addMenu("切换其他时间段的log");
    //        QAction *openTmLog = new QAction("打开对应的TestMaster log", logs);
    //        connect(openTmLog, &QAction::triggered, this, [=]()
    //                { onAddItemTriggered(openTmLog); });

    //        for (const QString &startTime : allLogInfo[item->text(0)]["startTime"].toStringList())
    //        {
    //            QAction *addAction = new QAction(startTime, logs);

    //            // 将 item 作为上下文数据传递给 QAction 对象
    //            addAction->setData(QVariant::fromValue(item));

    //            // 连接信号和槽函数，并将 QAction 对象作为参数传递
    //            connect(addAction, &QAction::triggered, this, [=]()
    //                    { onAddItemTriggered(addAction); });

    //            logs->addAction(addAction);
    //        }
    //        menu->addAction(openTmLog);
    //        menu->exec(ui->treeWidget_SearchResult->mapToGlobal(pos));
}

void LogAnalysis::openXML(const QString &xml)
{
    LPCWSTR url = reinterpret_cast<LPCWSTR>(xml.utf16());

    // 使用 ShellExecute 函数打开文件
    intptr_t result = reinterpret_cast<intptr_t>(ShellExecute(NULL, L"open", L"iexplore.exe", url, NULL, SW_SHOWNORMAL));

    if (result <= 32)
    {
        // 打开失败，可以根据具体错误进行处理
        // 使用 GetLastError() 获取错误代码
        qDebug() << xml << "open failed!" << GetLastError();
    }
}

void LogAnalysis::onAddItemTriggered(QAction *action)
{

    // 获取 QAction 对象中保存的上下文数据
    QTreeWidgetItem *item = action->data().value<QTreeWidgetItem *>();

    qDebug() << "当前选中：" + item->text(0);

    // 同时更新子控件下对应的所有文件信息
    // ...
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

// 抓取测试套的统计信息
QStringList LogAnalysis::getTestSuiteInfo(const QString &xml)
{
    QStringList matchList;
    QFile file(xml);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug() << "failed to open" << xml;

        return matchList;
    }

    QString xmlContent;

    while (!file.atEnd())
    {
        xmlContent.append(file.readLine() + "\n");
    }

    static QRegularExpression re_TIME("<TIME>(.*?)</TIME>");
    static QRegularExpression re_UseTime("<CONTENT type = \"text\"><!\\[CDATA\\[.*:(\\d+\\.\\d+)\\s.*\\]\\]></CONTENT>");
    static QRegularExpression re_OK_Num("<SECTION>\\s*<TITLE>OK<\\/TITLE>\\s*<VALUE><!\\[CDATA\\[(\\d+)\\]\\]><\\/VALUE>\\s*<\\/SECTION>");
    static QRegularExpression re_NG_Num("<SECTION>\\s*<TITLE>NG<\\/TITLE>\\s*<VALUE><!\\[CDATA\\[(\\d+)\\]\\]><\\/VALUE>\\s*<\\/SECTION>");
    static QRegularExpression re_ER_Num("<SECTION>\\s*<TITLE>ER<\\/TITLE>\\s*<VALUE><!\\[CDATA\\[(\\d+)\\]\\]><\\/VALUE>\\s*<\\/SECTION>");
    static QRegularExpression re_InvalidHead_Num("<SECTION>\\s*<TITLE>Invalid Head<\\/TITLE>\\s*<VALUE><!\\[CDATA\\[(\\d+)\\]\\]><\\/VALUE>\\s*<\\/SECTION>");

    if (re_UseTime.match(xmlContent).hasMatch())
    {
        matchList << re_UseTime.match(xmlContent).captured(1) + "h";
        matchList << (re_OK_Num.match(xmlContent).hasMatch() ? re_OK_Num.match(xmlContent).captured(1) : "");
        matchList << (re_NG_Num.match(xmlContent).hasMatch() ? re_NG_Num.match(xmlContent).captured(1) : "");
        matchList << (re_ER_Num.match(xmlContent).hasMatch() ? re_ER_Num.match(xmlContent).captured(1) : "");
        matchList << (re_InvalidHead_Num.match(xmlContent).hasMatch() ? re_InvalidHead_Num.match(xmlContent).captured(1) : "");
    }
    else
    {
        QStringList timeList;
        QRegularExpressionMatchIterator matchAllTime = re_TIME.globalMatch(xmlContent);
        while (matchAllTime.hasNext())
        {
            timeList << matchAllTime.next().captured(1);
        }

        if (!timeList.isEmpty())
        {
            qDebug() << "*******************************************************************";
            qDebug() << "old time:" << timeList.first();
            qDebug() << "new time:" << timeList.last();

            QDateTime oldDateTime = QDateTime::fromString(timeList.first(), "yyyy-MM-dd hh:mm:ss");
            QDateTime newDateTime = QDateTime::fromString(timeList.last(), "yyyy-MM-dd hh:mm:ss");

            qint64 oldTimestamp = oldDateTime.toMSecsSinceEpoch();
            qint64 newTimestamp = newDateTime.toMSecsSinceEpoch();
            qDebug() << "oldTimestamp:" << oldTimestamp;
            qDebug() << "newTimestamp:" << newTimestamp;

            qint64 diffMilliseconds = newTimestamp - oldTimestamp;
            qint64 hours = diffMilliseconds / (1000 * 60 * 60); // 将毫秒转换为小时

            qDebug() << "Count output time:" << hours;

            matchList << QString::number(hours) + "h used NotCompleted";
        }
        else
        {
            matchList << "Not started";
        }
    }

    return matchList;
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

// 从log文件中获取所有tcl脚本信息
QStringList LogAnalysis::getAllTclFromTestSuite(const QString &xmlPath)
{
    QStringList tclScripts;
    static QRegularExpression re_ExecuteTclFile("<a href=.*>(.*?)</a>");
    // 打开xml文件并逐行读取记录的 tcl文件路径
    QFile file(xmlPath);
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
        qDebug() << xmlPath << "does not exist or cannot be opened";
    }
    return tclScripts;
}

// 获取一个目录下的所有测试集的log文件，文件名最短的xml文件，即为TestSuite的log文件
QStringList LogAnalysis::getAllTestSuiteXML(const QString &dir)
{
    QStringList xmlNameList = QDir(dir).entryList(QStringList() << "*.xml", QDir::Files);
    if (xmlNameList.isEmpty())
    {
        qDebug() << dir << "does not have an xml file";
        return QStringList();
    }

    QList<int> nameLenList;

    for (const QString &xml : xmlNameList)
    {
        nameLenList << xml.length();
    }

    std::sort(nameLenList.begin(), nameLenList.end());
    short int maxLen = nameLenList[0];
    QStringList testSuite;

    for (const QString &xml : xmlNameList)
    {
        if (xml.length() == maxLen)
        {
            testSuite << QDir(dir).filePath(xml);
        }
    }

    return testSuite;
}

void LogAnalysis::onTriggered(const QPoint &pos)
{
}
#include <QMenuBar>
void LogAnalysis::updateTreeWidget()
{
    ui->treeWidget_SearchResult->clear();

    QDir dir(ui->lineEdit_Dir->text());
    dir.setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    QDirIterator it(dir, QDirIterator::Subdirectories);
    QMap<QString, QMap<QString, QVariant>> map;

    QStringList allTestSuite;

    int totalFileNum = 0;
    // 迭代遍历所有目录
    while (it.hasNext())
    {
        allTestSuite = getAllTestSuiteXML(it.next());

        // 给所有xml文件按照时间排序
        std::sort(allTestSuite.begin(), allTestSuite.end(), [](const QString &a, const QString &b)
                  { return a.mid(a.lastIndexOf("_") + 1, 14) > b.mid(a.lastIndexOf("_") + 1, 14); });

        if (!allTestSuite.isEmpty())
        {
            QString xmlPath = allTestSuite[0];
            QFileInfo file(xmlPath);
            QString fileName = file.fileName().left(file.fileName().lastIndexOf("_"));
            QStringList tclFileList;

            map[fileName]["path"] = xmlPath;
            map[fileName]["infoList"] = getTestSuiteInfo(xmlPath);

            for (const QString &tclFile : getAllTclFromTestSuite(xmlPath))
            {
                ++totalFileNum;
                tclFileList << tclFile;
            }
            map[fileName]["tclList"] = tclFileList;
        }
    }

    // 创建一个进度条
    QProgressDialog progressDialog("查找文件...", "Cancel", 0, totalFileNum, this);
    progressDialog.setWindowModality(Qt::WindowModal);
    progressDialog.setAutoClose(true);
    progressDialog.setAutoReset(true);
    progressDialog.autoReset();
    progressDialog.setMinimumDuration(0);
    progressDialog.setStyleSheet("QProgressBar {"
                                 "    border-radius: 7px;"
                                 "    border: 3px solid  #4CE637;"
                                 "    height: 20px;"
                                 "    border-radius: 13px;"
                                 "    text-align: center;"
                                 "}"
                                 "QProgressBar::chunk {"
                                 "    background-color: #FAE63C;"
                                 "    margin: 6px;" // 向内缩小涂色部分
                                 "}");
    totalFileNum = 0;
    QStringList xmls = map.keys();


    connect(ui->treeWidget_SearchResult, &QTreeWidget::customContextMenuRequested, this, [&](const QPoint &pos)
            {
                QTreeWidgetItem *item = ui->treeWidget_SearchResult->itemAt(pos);
                qDebug() << item;
                qDebug() << item->data(0,Qt::UserRole);
                if (item)
                {
                    QMenu *menu1 = new QMenu("JOJO", ui->treeWidget_SearchResult);
                    QMenu *menu2 = new QMenu("DIO", menu1);
                    QAction *actionGIOGIO = new QAction("GIOGIO", menu2);
                    connect(actionGIOGIO, &QAction::triggered, this, []()
                            { qDebug() << "GIOGIO"; });
                    menu2->addAction(actionGIOGIO);
                    menu1->addMenu(menu2);
                    menu1->exec(ui->treeWidget_SearchResult->viewport()->mapToGlobal(pos));
                    delete menu1;
                }
            });

    for (const QString &xml : xmls)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget_SearchResult);
        QString absPath = map[xml]["path"].toString();

        item->setText(0, xml);
        item->setIcon(0, QIcon(":/ico/tst.ico"));
        item->setToolTip(0, "右键可打开\n" + absPath);

        // 将右键菜单关联到树形控件的每个项
        ui->treeWidget_SearchResult->setContextMenuPolicy(Qt::CustomContextMenu);

        int i = 1;
        for (const QString &info : map[xml]["infoList"].toStringList())
        {
            item->setText(i, info);
            item->setTextAlignment(i, Qt::AlignCenter);
            ++i;
        }

        qDebug() << "# ///////////////////////////////////////////////////////////////////// #";
        qDebug() << "into " << absPath;

        // 添加子item
        for (const QString &tcl : map[xml]["tclList"].toStringList())
        {
            QTreeWidgetItem *tclItem = new QTreeWidgetItem(item);
            ++totalFileNum;
            tclItem->setText(0, tcl);
            tclItem->setIcon(0, QIcon(":/ico/file_tcl.ico"));
            progressDialog.setValue(totalFileNum);
            progressDialog.setLabelText("填充:" + tcl);
            qDebug() << "add " << tcl;
        }
    }
}
