#include "loganalysis.h"
#include "ui_loganalysis.h"

LogAnalysis::LogAnalysis(QWidget *parent) : QWidget(parent), ui(new Ui::LogAnalysis)
{
    ui->setupUi(this);
    isExpandAll = false;
    ui->pushButton_ExpandAll->setIconSize(QSize(32, 32));
    ui->pushButton_SelectDir->setIcon(QIcon(":/icon/folder.ico"));
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
                    ui->pushButton_ExpandAll->setIcon(QIcon(":/icon/Collapse.ico"));
                    isExpandAll = true;
                }
                else
                {
                    ui->treeWidget_SearchResult->collapseAll();
                    ui->pushButton_ExpandAll->setText("展开");
                    ui->pushButton_ExpandAll->setIcon(QIcon(":/icon/Expand.ico"));
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

    // 绑定树控件的右键为自定义函数
    connect(ui->treeWidget_SearchResult, &QTreeWidget::customContextMenuRequested, this, &LogAnalysis::onTriggered);
    // 将右键菜单关联到树形控件的每个项
    ui->treeWidget_SearchResult->setContextMenuPolicy(Qt::CustomContextMenu);

    ui->lineEdit_Dir->setText("D:\\log");

    //    QString file = "D:\\测试转发组脚本集\\log\\GRE\\TestMaster_TS_BAS_GRE_10.1.11.41.2.13_1_1_4_20230426092744.xml";
    //    getTestSuiteLogContent(file);
}

LogAnalysis::~LogAnalysis()
{
    delete ui;
}

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

// 抓取测试套的log信息
QMap<QString, QString> LogAnalysis::getTestSuiteLogInfo(const QString &xml)
{
    QMap<QString, QString> map;
    QFile file(xml);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug() << "failed to open" << xml;

        return map;
    }

    QString xmlContent;

    while (!file.atEnd())
    {
        xmlContent.append(file.readLine() + "\n");
    }

    static QRegularExpression re_StartTIME("<TIME>(.*?)</TIME>");
    static QRegularExpression re_UseTime("<CONTENT type = \"text\"><!\\[CDATA\\[.*:(\\d+\\.\\d+)\\s.*\\]\\]></CONTENT>");
    static QRegularExpression re_OK_Num("<SECTION>\\s*<TITLE>OK<\\/TITLE>\\s*<VALUE><!\\[CDATA\\[(\\d+)\\]\\]><\\/VALUE>\\s*<\\/SECTION>");
    static QRegularExpression re_NG_Num("<SECTION>\\s*<TITLE>NG<\\/TITLE>\\s*<VALUE><!\\[CDATA\\[(\\d+)\\]\\]><\\/VALUE>\\s*<\\/SECTION>");
    static QRegularExpression re_ER_Num("<SECTION>\\s*<TITLE>ER<\\/TITLE>\\s*<VALUE><!\\[CDATA\\[(\\d+)\\]\\]><\\/VALUE>\\s*<\\/SECTION>");
    static QRegularExpression re_InvalidHead_Num("<SECTION>\\s*<TITLE>Invalid Head<\\/TITLE>\\s*<VALUE><!\\[CDATA\\[(\\d+)\\]\\]><\\/VALUE>\\s*<\\/SECTION>");
    static QRegularExpression re_Completed("</ATFLOG>");

    if (re_UseTime.match(xmlContent).hasMatch())
    {
        map["UseTime"] = re_UseTime.match(xmlContent).captured(1);
    }
    else
    {
        map["UseTime"] = "unfinished";

        //        QStringList timeList;
        //        QRegularExpressionMatchIterator matchAllTime = re_TIME.globalMatch(xmlContent);
        //        while (matchAllTime.hasNext())
        //        {
        //            timeList << matchAllTime.next().captured(1);
        //        }

        //        if (!timeList.isEmpty())
        //        {
        //            qDebug() << "*******************************************************************";
        //            qDebug() << "old time:" << timeList.first();
        //            qDebug() << "new time:" << timeList.last();

        //            QDateTime oldDateTime = QDateTime::fromString(timeList.first(), "yyyy-MM-dd hh:mm:ss");
        //            QDateTime newDateTime = QDateTime::fromString(timeList.last(), "yyyy-MM-dd hh:mm:ss");

        //            qint64 oldTimestamp = oldDateTime.toMSecsSinceEpoch();
        //            qint64 newTimestamp = newDateTime.toMSecsSinceEpoch();
        //            qDebug() << "oldTimestamp:" << oldTimestamp;
        //            qDebug() << "newTimestamp:" << newTimestamp;

        //            qint64 diffMilliseconds = newTimestamp - oldTimestamp;
        //            qint64 hours = diffMilliseconds / (1000 * 60 * 60); // 将毫秒转换为小时

        //            qDebug() << "Count output time:" << hours;

        //            matchList << QString::number(hours) + "h used NotCompleted";
        //        }
        //        else
        //        {
        //            matchList << "Not started";
        //        }
    }
    map["StartTime"] = (re_StartTIME.match(xmlContent).hasMatch() ? re_StartTIME.match(xmlContent).captured(1) : "");
    map["OK"] = (re_OK_Num.match(xmlContent).hasMatch() ? re_OK_Num.match(xmlContent).captured(1) : "");
    map["NG"] = (re_NG_Num.match(xmlContent).hasMatch() ? re_NG_Num.match(xmlContent).captured(1) : "");
    map["ER"] = (re_ER_Num.match(xmlContent).hasMatch() ? re_ER_Num.match(xmlContent).captured(1) : "");
    map["InvalidHead"] = (re_InvalidHead_Num.match(xmlContent).hasMatch() ? re_InvalidHead_Num.match(xmlContent).captured(1) : "");
    map["isCompleted"] = (re_Completed.match(xmlContent).hasMatch() ? "Completed" : "Incomplete");
    return map;
}

// 抓取测试脚本的log信息
QMap<QString, QString> LogAnalysis::getScriptLogInfo(const QString &filePath)
{
    QMap<QString, QString> map;
    QFile file(filePath);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug() << "failed to open" << filePath;
        return QMap<QString, QString>{};
    }
    QString xmlContent;
    QTextStream in(&file);
    while (!in.atEnd())
    {
        xmlContent.append(in.readLine() + "\n");
    }
    file.close();

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

// 从log文件中获取所有tcl脚本信息（包括setup、clear脚本）
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

// 返回一个目录下的所有测试集文件列表（代码逻辑：文件名最短的xml文件，即为TestSuite的log文件，默认按照时间排序）
QStringList LogAnalysis::getAllTestSuiteXML(const QString &dir)
{
    QDir *Dir = new QDir(dir);

    // 默认不对topos文件夹进行检索，以避免无意义的log搜索，以提高性能
    if (Dir->dirName() == "topos")
    {
        return QStringList();
    }

    QStringList xmlNameList = Dir->entryList(QStringList() << "*.xml", QDir::Files);
    if (xmlNameList.isEmpty())
    {
        qDebug() << __func__ << "warning: " << dir << "has no xml files";
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

    // 给所有log文件按照时间排序
    std::sort(testSuite.begin(), testSuite.end(), [](const QString &a, const QString &b)
              { return a.mid(a.lastIndexOf("_") + 1, 14) > b.mid(a.lastIndexOf("_") + 1, 14); });

    return testSuite;
}

// 当发生控件右键点击时，执行该函数
void LogAnalysis::onTriggered(const QPoint &pos)
{
    // 获取传入pos所在位置的QTreeWidgetItem指针
    QTreeWidgetItem *item = ui->treeWidget_SearchResult->itemAt(pos);

    if (item && item->data(column::ID, Qt::UserRole).toString() == "father" && item->text(0) != "总计")
    {
        QMenu *menu = new QMenu(ui->treeWidget_SearchResult);
        QMenu *menuOpen = new QMenu("打开", ui->treeWidget_SearchResult);
        QMenu *selectOtherLog = new QMenu("其他时间log", menu);
        QAction *actionDelete = new QAction("删除", menu);
        QAction *actionOpenXml = new QAction("打开xml", menuOpen);
        QAction *actionOpenTMXml = new QAction("打开TestMaster xml", menuOpen);

        menuOpen->setIcon(QIcon(":/icon/icons8-xml-100.png"));
        selectOtherLog->setIcon(QIcon(":/icon/icons8-time-80.png"));
        actionDelete->setIcon(QIcon(":/icon/icons8-delete-100.png"));

        connect(actionDelete, &QAction::triggered, this, [=]()
                {total_UseTime -= item->data(column::UseTime,Qt::UserRole).toFloat();
                total_OK -= item->data(column::OK,Qt::UserRole).toInt();
                total_NG -= item->data(column::NG,Qt::UserRole).toInt();
                total_ER -= item->data(column::ER,Qt::UserRole).toInt();
                total_InvalidHead -= item->data(column::InvalidHead,Qt::UserRole).toInt();
                delete item;
                updateItemTotal();
                });
        connect(actionOpenXml, &QAction::triggered, this, [=]()
                { showXMLInIE(item->data(column::XmlPath, Qt::UserRole).toString()); });
        connect(actionOpenTMXml, &QAction::triggered, this, [=]()
                { showXMLInIE(item->data(column::TMXmlPath, Qt::UserRole).toString()); });

        bool first = true;

        for (QString &data : item->data(0, Qt::UserRole).toStringList())
        {
            QFileInfo file(data);
            short int lastIdx = file.fileName().lastIndexOf("_");
            QString fileNameTimeString = file.fileName().mid(lastIdx + 1, 14);
            QString timeString = QDateTime::fromString(fileNameTimeString, "yyyyMMddhhmmss").toString("yyyy-MM-dd hh:mm:ss");

            QAction *switchLog = new QAction(timeString, selectOtherLog);
            if (first)
            {
                switchLog->setIcon(QIcon(":/icon/select.ico"));
                first = false;
            }
            connect(switchLog, &QAction::triggered, this, [timeString]()
                    { qDebug() << "select:" << timeString; });
            selectOtherLog->addAction(switchLog);
        }

        menu->addAction(actionDelete);
        menu->addMenu(selectOtherLog);
        menu->addMenu(menuOpen);
        menuOpen->addAction(actionOpenXml);
        menuOpen->addAction(actionOpenTMXml);

        menu->exec(ui->treeWidget_SearchResult->viewport()->mapToGlobal(pos));
        delete menu;
    }
    else if (item && item->data(column::ID, Qt::UserRole).toString() == "son")
    {
    }
}

// 遍历目录并生成Tst映射字典
QMap<QString, QMap<QString, QVariant>> LogAnalysis::traverseDirCreateTstMap()
{
    QDir dir(ui->lineEdit_Dir->text());
    dir.setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    QDirIterator directory(dir, QDirIterator::Subdirectories);
    QMap<QString, QMap<QString, QVariant>> map;

    // 迭代遍历所有目录
    while (directory.hasNext())
    {
        QString dirPath = directory.next();
        QStringList testSuiteList = getAllTestSuiteXML(dirPath);
        if (!testSuiteList.isEmpty())
        {
            QStringList tclScriptList;
            QString xmlPath;

            // 遍历所有log文件，仅获取已完成的log
            for (QString &path : testSuiteList)
            {
                QMap<QString, QString> logInfo = getTestSuiteLogInfo(path);

                if (logInfo["isCompleted"] == "Completed")
                {
                    xmlPath = path;
                    break;
                }
                xmlPath = path;
            }
            QFileInfo file(xmlPath);
            QString testSuiteName = file.fileName().left(file.fileName().lastIndexOf("_"));
            QString TMXmlPath = dirPath + "/TestMaster_" + file.fileName();

            qDebug() << "TMXmlPath:" << TMXmlPath;

            map[testSuiteName]["xmlPath"] = xmlPath;
            map[testSuiteName]["TMXmlPath"] = TMXmlPath;
            map[testSuiteName]["allTestSuiteList"] = testSuiteList;

            for (const QString &tclFile : getAllTclFromTestSuite(xmlPath))
            {
                ++progressDialog_Count;
                tclScriptList << tclFile;
            }
            map[testSuiteName]["tclScriptList"] = tclScriptList;
        }
    }
    return map;
}

// 返回一个预设好的QProgressDialog进度条
QProgressDialog *LogAnalysis::progressDialog()
{
    // 创建一个进度条
    QProgressDialog *p = new QProgressDialog(this);
    p->setCancelButtonText("Cancel");
    p->setLabelText("遍历文件...");
    p->setWindowModality(Qt::WindowModal);
    p->setAutoClose(true);
    p->setAutoReset(true);
    p->autoReset();
    p->setMaximum(progressDialog_Count);
    p->setMinimumDuration(0);
    p->setStyleSheet("QProgressBar {"
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
    return p;
}

// 调用IE浏览器打开xml文件
void LogAnalysis::showXMLInIE(const QString &xmlPath)
{
    LPCWSTR url = reinterpret_cast<LPCWSTR>(xmlPath.utf16());

    // 使用 ShellExecute 函数调用 IE浏览器打开文件
    intptr_t result = reinterpret_cast<intptr_t>(ShellExecute(NULL, L"open", L"iexplore.exe", url, NULL, SW_SHOWNORMAL));

    if (result <= 32)
    {
        // 打开失败，可以根据具体错误进行处理
        // 使用 GetLastError() 获取错误代码
        qDebug() << xmlPath << "open failed!" << GetLastError();
    }
}

void LogAnalysis::updateItemTotal()
{
    int lastItem = ui->treeWidget_SearchResult->topLevelItemCount() -1;
    qDebug() <<"lastItem"<<lastItem;
    QTreeWidgetItem *itemTotal = ui->treeWidget_SearchResult->topLevelItem(lastItem);
    if (itemTotal)
    {
        int totalNum = total_OK + total_NG + total_ER + total_InvalidHead;

        itemTotal->setText(column::TestSuiteName, "总计");
        itemTotal->setText(column::UseTime, QString::number(total_UseTime) + "h");
        itemTotal->setText(column::OK, QString::number(total_OK));
        itemTotal->setText(column::NG, QString::number(total_NG));
        itemTotal->setText(column::ER, QString::number(total_ER));
        itemTotal->setText(column::InvalidHead, QString::number(total_InvalidHead));

        if (totalNum == 0)
        {
            itemTotal->setText(column::TotalNum, "");
            itemTotal->setText(column::PassRate, "");
        }
        else
        {
            itemTotal->setText(column::TotalNum, QString::number(totalNum));
            itemTotal->setText(column::PassRate, QString::number(((float)total_OK / totalNum) * 100, 'f', 0) + "%");
        }

        for (int i = 0; i <= len_itemColumn; ++i)
        {
            itemTotal->setBackground(i, QBrush(QColor(175, 216, 241)));
        }
        for (int i = 1; i <= len_itemColumn; ++i)
        {
            itemTotal->setTextAlignment(i, Qt::AlignCenter);
        }

    }
}

void LogAnalysis::updateTreeWidget()
{
    ui->treeWidget_SearchResult->clear();

    total_UseTime = 0;
    total_OK = 0;
    total_NG = 0;
    total_ER = 0;
    total_InvalidHead = 0;
    len_itemColumn = 9;
    progressDialog_Count = 0;

    QMap<QString, QMap<QString, QVariant>> map = traverseDirCreateTstMap();
    QProgressDialog *probar = progressDialog();

    // 置零
    progressDialog_Count = 0;

    QStringList xmls = map.keys();

    for (const QString &xml : xmls)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget_SearchResult);
        QString xmlPath = map[xml]["xmlPath"].toString();
        item->setText(column::TestSuiteName, xml);
        item->setIcon(column::TestSuiteName, QIcon(":/icon/icons8-oak-tree-48.png"));
        item->setToolTip(column::TestSuiteName, xmlPath);
        item->setData(column::TestSuiteName, Qt::UserRole, map[xml]["allTestSuiteList"]);
        item->setData(column::ID, Qt::UserRole, "father");
        item->setData(column::XmlPath, Qt::UserRole, xmlPath);
        item->setData(column::TMXmlPath, Qt::UserRole, xmlPath);

        QMap<QString, QString> testSuiteInfo = getTestSuiteLogInfo(xmlPath);

        float usetime = testSuiteInfo["UseTime"].toFloat();
        int num_OK = testSuiteInfo["OK"].toInt();
        int num_NG = testSuiteInfo["NG"].toInt();
        int num_ER = testSuiteInfo["ER"].toInt();
        int num_InvalidHead = testSuiteInfo["InvalidHead"].toInt();
        int totalNum = num_OK + num_NG + num_ER + num_InvalidHead;

        item->setText(column::StartTime, testSuiteInfo["StartTime"]);
        item->setText(column::UseTime, testSuiteInfo["UseTime"] + "h");
        item->setText(column::TotalNum, QString::number(totalNum));
        item->setText(column::OK, testSuiteInfo["OK"]);
        item->setText(column::NG, testSuiteInfo["NG"]);
        item->setText(column::ER, testSuiteInfo["ER"]);
        item->setText(column::InvalidHead, testSuiteInfo["InvalidHead"]);

        item->setData(column::UseTime, Qt::UserRole, usetime);
        item->setData(column::TotalNum, Qt::UserRole, totalNum);
        item->setData(column::OK, Qt::UserRole, num_OK);
        item->setData(column::NG, Qt::UserRole, num_NG);
        item->setData(column::ER, Qt::UserRole, num_ER);
        item->setData(column::InvalidHead, Qt::UserRole, num_InvalidHead);

        if (totalNum == 0)
        {
            item->setText(column::PassRate, "");
        }
        else
        {
            item->setText(column::PassRate, QString::number(((float)num_OK / totalNum) * 100, 'f', 0) + "%");
        }

        if (totalNum == num_OK)
        {
            // 全Pass的刷绿色
            for (int i = 0; i <= len_itemColumn; ++i)
            {
                item->setBackground(i, QBrush(QColor(234, 254, 228)));
            }
        }
        else
        {
            // 非全 OK的均刷红
            for (int i = 0; i <= len_itemColumn; ++i)
            {
                item->setBackground(i, QBrush(QColor(254, 228, 228)));
            }
        }
        for (int i = 1; i <= len_itemColumn; ++i)
        {
            item->setTextAlignment(i, Qt::AlignCenter);
        }

        // 总数追加
        total_UseTime += usetime;
        total_OK += num_OK;
        total_NG += num_NG;
        total_ER += num_ER;
        total_InvalidHead += num_InvalidHead;

        // 添加子item
        for (const QString &tcl : map[xml]["tclScriptList"].toStringList())
        {
            QTreeWidgetItem *tclItem = new QTreeWidgetItem(item);
            ++progressDialog_Count;
            tclItem->setText(column::TestSuiteName, tcl);
            tclItem->setIcon(column::TestSuiteName, QIcon(":/icon/icons8-file-64 (1).png"));
            tclItem->setData(column::ID, Qt::UserRole, "son");
            probar->setValue(progressDialog_Count);
            probar->setLabelText("添加：" + tcl);
        }
    }

//创建行尾统计栏
    QTreeWidgetItem *totalItem = new QTreeWidgetItem(ui->treeWidget_SearchResult);
totalItem->setText(column::TestSuiteName, "总计");
    updateItemTotal();
    delete probar;
}
