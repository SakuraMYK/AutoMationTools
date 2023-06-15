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


    // setSectionsClickable 不开起，那么QHeaderView::sectionClicked信号将无法触发
    ui->treeWidget_SearchResult->header()->setSectionsClickable(true);
    connect(ui->treeWidget_SearchResult->header(), &QHeaderView::sectionClicked, this, &LogAnalysis::sortItems);


}

LogAnalysis::~LogAnalysis()
{
    delete ui;
}

// 调用IE浏览器打开xml文件
void LogAnalysis::openXML(const QString &xmlPath)
{
    if (xmlPath.isEmpty())
    {
        qDebug() << __func__ << "warning:"
                 << "path is Empty!";
        return;
    }

    LPCWSTR url = reinterpret_cast<LPCWSTR>(xmlPath.utf16());

    // 使用 ShellExecute 函数打开文件
    intptr_t result = reinterpret_cast<intptr_t>(ShellExecute(NULL, L"open", L"iexplore.exe", url, NULL, SW_SHOWNORMAL));

    if (result <= 32)
    {
        // 打开失败，可以根据具体错误进行处理
        // 使用 GetLastError() 获取错误代码
        qDebug() << __func__ << "warning:" << xmlPath << "open failed!" << GetLastError();
    }
}

// 抓取测试套的log信息
QMap<QString, QString> LogAnalysis::getTestSuiteLogInfo(const QString &xml)
{
    QMap<QString, QString> map;
    QFile file(xml);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug() << __func__ << "warning: failed to open" << xml;

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

// 从log文件中获取所有tcl脚本的简单信息（包括setup、clear脚本）
QMap<QString, QStringList> LogAnalysis::getAllScriptInfoFromTestSuiteLog(const QString &xmlPath)
{
    QMap<QString, QStringList> map;

    // 打开xml文件并逐行读取记录的 tcl文件路径
    QFile file(xmlPath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QString xmlContent;
        QTextStream in(&file);
        while (!in.atEnd())
        {
            xmlContent.append(in.readLine() + "\n");
        }
        file.close();

        QStringList tclXmlNameList;
        QStringList tclTitleList;
        QStringList tclResultList;
        QStringList tclBeginList;

        static QRegularExpression re_ExecuteFile("<TITLE>Execute</TITLE>\\s+<VALUE><!\\[CDATA\\[<a href=\"\\./(.*?)\" target=\"_blank\">.*?</a>");
        static QRegularExpression re_Title("<TITLE>Title</TITLE>\\s+<VALUE><!\\[CDATA\\[(.*)\\]\\]></VALUE>");
        static QRegularExpression re_Result("<TITLE>Result</TITLE>\\s+<VALUE><!\\[CDATA\\[(\\w+)\\]\\]></VALUE>");
        static QRegularExpression re_Begin("<TITLE>Begin</TITLE>\\s+<VALUE><!\\[CDATA\\[(.*)\\]\\]></VALUE>");

        QRegularExpressionMatchIterator it_File = re_ExecuteFile.globalMatch(xmlContent);
        QRegularExpressionMatchIterator it_Title = re_Title.globalMatch(xmlContent);
        QRegularExpressionMatchIterator it_Result = re_Result.globalMatch(xmlContent);
        QRegularExpressionMatchIterator it_Begin = re_Begin.globalMatch(xmlContent);

        while (it_File.hasNext())
        {
            tclXmlNameList << it_File.next().captured(1);
        }
        while (it_Title.hasNext())
        {
            tclTitleList << it_Title.next().captured(1);
        }
        while (it_Result.hasNext())
        {
            tclResultList << it_Result.next().captured(1);
        }
        while (it_Begin.hasNext())
        {
            tclBeginList << it_Begin.next().captured(1);
        }

        map["tclXmlNameList"] = tclXmlNameList;
        map["tclTitleList"] = tclTitleList;
        map["tclResultList"] = tclResultList;
        map["tclBeginList"] = tclBeginList;
    }
    else
    {
        qDebug() << __func__ << "warning: " << xmlPath << "does not exist or cannot be opened";
    }
    return map;
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
    if (item && item->data(column::ID, Qt::UserRole).toString() == "father" && !item->text(0).contains("总计"))
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
                updateItemTotal(); });
        connect(actionOpenXml, &QAction::triggered, this, [=]()
                { openXML(item->data(column::XmlPath, Qt::UserRole).toString()); });
        connect(actionOpenTMXml, &QAction::triggered, this, [=]()
                { openXML(item->data(column::TMXmlPath, Qt::UserRole).toString()); });

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
        QMenu *menu = new QMenu(ui->treeWidget_SearchResult);
        QMenu *menuOpen = new QMenu("打开", ui->treeWidget_SearchResult);
        QAction *actionOpenXml = new QAction("打开xml", menuOpen);
        QAction *actionOpenTMXml = new QAction("打开TestMaster xml", menuOpen);

        menuOpen->setIcon(QIcon(":/icon/icons8-xml-100.png"));

        connect(actionOpenXml, &QAction::triggered, this, [=]()
                {
            qDebug() << "item->data(column::XmlPath, Qt::UserRole).toString()"<< item->data(column::XmlPath, Qt::UserRole).toString();
        openXML(item->data(column::XmlPath, Qt::UserRole).toString()); });
        connect(actionOpenTMXml, &QAction::triggered, this, [=]()
                { openXML(item->data(column::TMXmlPath, Qt::UserRole).toString()); });
        menu->addMenu(menuOpen);
        menuOpen->addAction(actionOpenXml);
        menuOpen->addAction(actionOpenTMXml);

        menu->exec(ui->treeWidget_SearchResult->viewport()->mapToGlobal(pos));
        delete menu;
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
            QMap<QString, QStringList> map_tclScript = getAllScriptInfoFromTestSuiteLog(xmlPath);

            map[testSuiteName]["dirPath"] = dirPath;
            map[testSuiteName]["xmlPath"] = xmlPath;
            map[testSuiteName]["TMXmlPath"] = TMXmlPath;
            map[testSuiteName]["allTestSuiteList"] = testSuiteList;

            map[testSuiteName]["tclXmlNameList"] = map_tclScript["tclXmlNameList"];
            map[testSuiteName]["tclTitleList"] = map_tclScript["tclTitleList"];
            map[testSuiteName]["tclResultList"] = map_tclScript["tclResultList"];
            map[testSuiteName]["tclBeginList"] = map_tclScript["tclBeginList"];

            progressDialog_Count += map_tclScript["xmlPathList"].length();
        }
    }
    return map;
}

// 返回一个预设的QProgressDialog进度条
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




// 更新TreeWigdet最后一行的item，即更新总计
void LogAnalysis::updateItemTotal( )
{
    //获取最后一行的item指针
    QTreeWidgetItem *itemTotal = ui->treeWidget_SearchResult->topLevelItem(ui->treeWidget_SearchResult->topLevelItemCount() - 1);

    itemTotal->setIcon(0, QIcon(":/icon/icons8-sum-49.png"));

    if (itemTotal)
    {
        int totalNum = total_OK + total_NG + total_ER + total_InvalidHead;

        itemTotal->setText(column::TestSuiteName, "总计:  " + QString::number(total_TestSuiteNum) + " 个模块");
        itemTotal->setText(column::UseTime, QString::number(total_UseTime) + "h");
        itemTotal->setText(column::OK, QString::number(total_OK));
        itemTotal->setText(column::NG, QString::number(total_NG));
        itemTotal->setText(column::ER, QString::number(total_ER));
        itemTotal->setText(column::InvalidHead, QString::number(total_InvalidHead));

        if (totalNum != 0)
        {
            itemTotal->setText(column::TotalNum, QString::number(totalNum));
            itemTotal->setText(column::PassRate, QString::number(((float)total_OK / totalNum) * 100, 'f', 0) + "%");
        }
        else
        {
            itemTotal->setText(column::TotalNum, "");
            itemTotal->setText(column::PassRate, "");
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

void LogAnalysis::sortItems(int column)
{

    //删除最后一个item指针
    delete ui->treeWidget_SearchResult->topLevelItem(ui->treeWidget_SearchResult->topLevelItemCount() - 1);

    //进行排序
    if(sortOrder)
    {
        sortOrder =false;
        ui->treeWidget_SearchResult->sortByColumn(column,Qt::AscendingOrder);
        qDebug() << "change AscendingOrder";

    }
    else {
        sortOrder =true;
        ui->treeWidget_SearchResult->sortByColumn(column,Qt::DescendingOrder);
        qDebug() << "change DescendingOrder";

    }

    //加上新的空item
    ui->treeWidget_SearchResult->addTopLevelItem(new QTreeWidgetItem);

    //将总计信息更新在这个最后一行的新item上
    updateItemTotal( );
}

void LogAnalysis::updateTreeWidget()
{
    ui->treeWidget_SearchResult->clear();


    total_UseTime = 0;
    total_OK = 0;
    total_NG = 0;
    total_ER = 0;
    total_InvalidHead = 0;
    total_TestSuiteNum = 0;
    len_itemColumn = 9;
    progressDialog_Count = 0;

    QMap<QString, QMap<QString, QVariant>> map = traverseDirCreateTstMap();
    QProgressDialog *probar = progressDialog();

    // 置零
    progressDialog_Count = 0;

    QStringList testSuiteNames = map.keys();

    for (const QString &testSuiteName : testSuiteNames)
    {
        // 创建主item
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget_SearchResult);
        QString xmlPath = map[testSuiteName]["xmlPath"].toString();

        item->setText(column::TestSuiteName, testSuiteName);
        item->setIcon(column::TestSuiteName, QIcon(":/icon/icons8-oak-tree-48.png"));
        item->setToolTip(column::TestSuiteName, xmlPath);

        item->setData(column::TestSuiteName, Qt::UserRole, map[testSuiteName]["allTestSuiteList"]);
        item->setData(column::ID, Qt::UserRole, "father");
        item->setData(column::XmlPath, Qt::UserRole, xmlPath);
        item->setData(column::TMXmlPath, Qt::UserRole, map[testSuiteName]["TMXmlPath"].toString());

        QMap<QString, QString> testSuiteInfo = getTestSuiteLogInfo(xmlPath);

        float usetime = testSuiteInfo["UseTime"].toFloat();
        int num_OK = testSuiteInfo["OK"].toInt();
        int num_NG = testSuiteInfo["NG"].toInt();
        int num_ER = testSuiteInfo["ER"].toInt();
        int num_InvalidHead = testSuiteInfo["InvalidHead"].toInt();
        int totalNum = num_OK + num_NG + num_ER + num_InvalidHead;

        item->setText(column::StartTime, testSuiteInfo["StartTime"]);
        testSuiteInfo["UseTime"] == "unfinished" ? item->setText(column::UseTime, "unfinished") : item->setText(column::UseTime, testSuiteInfo["UseTime"] + "h");
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
        ++total_TestSuiteNum;

        QString dir = map[testSuiteName]["dirPath"].toString();
        QStringList tclXmlNameList = map[testSuiteName]["tclXmlNameList"].toStringList();
        QStringList tclTitleList = map[testSuiteName]["tclTitleList"].toStringList();
        QStringList tclResultList = map[testSuiteName]["tclResultList"].toStringList();
        QStringList tclBeginList = map[testSuiteName]["tclBeginList"].toStringList();

        // 防止数组越界导致程序崩溃，提取获取所有列表中的最小值，再遍历
        int minSize[4];

        minSize[0] = tclXmlNameList.length();
        minSize[1] = tclTitleList.length();
        minSize[2] = tclResultList.length();
        minSize[3] = tclBeginList.length();

        int min = minSize[0];
        for (short int i = 1; i < 4; ++i)
        {
            if (minSize[i] < min)
            {
                min = minSize[i];
            }
        }
        // 添加子item
        for (int idx = 0; idx < min; ++idx)
        {
            QTreeWidgetItem *tclItem = new QTreeWidgetItem(item);

            tclItem->setText(column::TestSuiteName, tclXmlNameList[idx].left(tclXmlNameList[idx].lastIndexOf('_')) + ".tcl");
            tclItem->setText(column::StartTime, tclBeginList[idx]);

            tclItem->setData(column::ID, Qt::UserRole, "son");
            tclItem->setData(column::XmlPath, Qt::UserRole, dir + "/" + tclXmlNameList[idx]);
            tclItem->setData(column::TMXmlPath, Qt::UserRole, dir + "/TestMaster_" + tclXmlNameList[idx]);

            if (tclResultList[idx] == "OK")
            {
                tclItem->setIcon(column::TestSuiteName, QIcon(":/icon/icons8-ok-48.png"));
            }
            else if (tclResultList[idx] == "NG")
            {
                tclItem->setIcon(column::TestSuiteName, QIcon(":/icon/icons8-n-40.png"));
            }
            else if (tclResultList[idx] == "ER")
            {
                tclItem->setIcon(column::TestSuiteName, QIcon(":/icon/error-100.png"));
            }
            else
            {
                tclItem->setIcon(column::TestSuiteName, QIcon(":/icon/icons8-file-64 (1).png"));
            }

            probar->setLabelText("添加：" + tclXmlNameList[idx]);
        }
        probar->setValue(progressDialog_Count);
        ++progressDialog_Count;
    }

    // 创建一个空的item作为行尾统计栏
    ui->treeWidget_SearchResult->addTopLevelItem(new QTreeWidgetItem);

    // 更新总计信息到这个空item里
    updateItemTotal();

    // 删除进度条
    delete probar;
}



