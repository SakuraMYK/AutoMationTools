#include "excelscripter.h"
#include "ui_excelscripter.h"

ExcelScripter::ExcelScripter(QWidget *parent) : QWidget(parent),
                                                ui(new Ui::ExcelScripter)
{

    ui->setupUi(this);
    this->resize(1000,800);
    connect(ui->pushButton_Read, &QPushButton::clicked, this, &ExcelScripter::updateTree);
    connect(ui->pushButton_Create, &QPushButton::clicked, this, &ExcelScripter::createScript);

    ui->pushButton_SplitDone->hide();

    ui->comboBox_SelectFile->setCurrentText("C:/Users/JOJO/Desktop/JOJO.xlsx");
    ui->pushButton_SelectAll->setIcon(QIcon(":/icon/icons8-circle-96.png"));

    connect(ui->treeWidget, &QTreeWidget::itemClicked, this, &ExcelScripter::itemClicked);
    connect(ui->pushButton_SplitByTopo, &QPushButton::clicked, this, &ExcelScripter::splitByTopo);
    connect(ui->pushButton_SplitDone, &QPushButton::clicked, this, &ExcelScripter::splitDone);
    connect(ui->pushButton_SelectAll, &QPushButton::clicked, this, &ExcelScripter::selectAllItem);
    connect(ui->comboBox_FilterHeadItem, &QComboBox::currentTextChanged, this, &ExcelScripter::titleComboBoxTextChanged);
    connect(ui->comboBox_FilterContent, &QComboBox::currentTextChanged, this, &ExcelScripter::filterComboBoxTextChanged);

    disableWidget();
}

ExcelScripter::~ExcelScripter()
{
    delete ui;
}

QMap<int, QStringList> ExcelScripter::readExcel()
{

    QMap<int, QStringList> testCase;
    QFileInfo fileInfo(ui->comboBox_SelectFile->currentText());
    if (!fileInfo.exists())
    {
        ui->label_Tips->setText("文件路径不存在!");
        QTimer::singleShot(1000, this, [this]()
                           { ui->label_Tips->setText(""); });
        return testCase;
    }
    if (!fileInfo.isFile())
    {
        ui->label_Tips->setText("请选择合法文件!");
        QTimer::singleShot(1000, this, [this]()
                           { ui->label_Tips->setText(""); });
        return testCase;
    }
    if (fileInfo.suffix() == "xlsx" || fileInfo.suffix() == "xls")
    {
    }
    else
    {
        ui->label_Tips->setText("请选择Excel文件!");
        QTimer::singleShot(1000, this, [this]()
                           { ui->label_Tips->setText(""); });
        return testCase;
    }

    int incrNum = 0;
    // 创建一个进度条
    QProgressDialog *p = new QProgressDialog(this);
    p->setLabelText("遍历文件...");
    p->setWindowTitle("读取Excel数据");
    p->setWindowModality(Qt::WindowModal);
    p->setAutoClose(true);
    p->setAutoReset(true);
    p->autoReset();
    p->setMaximum(4);
    p->setMinimumDuration(0);
    p->setStyleSheet("QProgressBar {"
                     "    border-radius: 7px;"
                     "    border: 3px solid  #BADAFB;"
                     "    height: 20px;"
                     "    border-radius: 13px;"
                     "    text-align: center;"
                     "}"
                     "QProgressBar::chunk {"
                     "    background-color: #E2BAFB;"
                     "    margin: 6px;" // 向内缩小涂色部分
                     "}");
    p->setMaximum(5);
    QString filePath = QDir::toNativeSeparators(fileInfo.absoluteFilePath());

    p->setLabelText("开始对接Excel");
    p->setValue(1);

    QAxObject excel("Excel.Application");
    excel.setProperty("Visible", false);
    excel.setProperty("DisplayAlerts", false);

    p->setLabelText("Excel.Application");
    p->setValue(2);

    QAxObject *workbooks = excel.querySubObject("Workbooks");
    QAxObject *workbook = workbooks->querySubObject("Open(const QString&)", filePath);

    p->setLabelText("Workbooks");
    p->setValue(3);

    QAxObject *sheets = workbook->querySubObject("Worksheets");
    QAxObject *sheet = sheets->querySubObject("Item(int)", 1); // 读取第一个sheet

    p->setLabelText("sheets");
    p->setValue(4);

    QAxObject *usedRange = sheet->querySubObject("UsedRange");
    QAxObject *rows = usedRange->querySubObject("Rows");
    QAxObject *columns = usedRange->querySubObject("Columns");

    p->setLabelText("UsedRange");
    p->setValue(5);

    int rowCount = rows->property("Count").toInt();
    int columnCount = columns->property("Count").toInt();

    p->setValue(0);
    p->setMaximum(rowCount * columnCount);

    struct cell
    {
        int row = 0;
        int column = 0;
    };
    struct cell moduleNumCell;
    struct cell moduleNameCell;
    struct cell testPointNumCell;

    for (int row = 1; row <= rowCount; ++row)
    {
        for (int column = 1; column <= columnCount; ++column)
        {
            QAxObject *cell = sheet->querySubObject("Cells(int,int)", row, column);
            QVariant value = cell->property("Value");
            if (value.isValid())
            {
                if (value.toString() == "模块编号")
                {
                    moduleNumCell.row = row;
                    moduleNumCell.column = column;
                }
                else if (value.toString() == "模块名")
                {
                    moduleNameCell.row = row;
                    moduleNameCell.column = column;
                }
                else if (value.toString() == "测试点编号")
                {
                    testPointNumCell.row = row;
                    testPointNumCell.column = column;
                }
                else
                {
                    if (moduleNumCell.row != 0 && moduleNumCell.column != 0 &&
                        moduleNameCell.row != 0 && moduleNameCell.column != 0 &&
                        testPointNumCell.row != 0 && testPointNumCell.column != 0)
                    {
                        break;
                    }
                }
            }
            else
            {
                //               qDebug() << "Cell (" << row << "," << column << ") 单元格值不合法";
            }
            delete cell;
            p->setLabelText("遍历单元格: (" + QString::number(row) + "," + QString::number(column) + ")");
            p->setValue(incrNum);
            ++incrNum;
        }
    }

    delete p;

    headColumnStart = moduleNumCell.column;
    headColumnEnd = columnCount;
    headRow = moduleNumCell.row;
    headTitleList.clear();
    for (int column = testPointNumCell.column; column < headColumnEnd; ++column)
    {
        QAxObject *cell = sheet->querySubObject("Cells(int,int)", headRow, column);
        QVariant value = cell->property("Value");
        if (value.isValid())
        {
            headTitleList << value.toString();
        }
        delete cell;
    }

    for (int row = moduleNumCell.row + 1; row <= rowCount; ++row)
    {
        QAxObject *cell = sheet->querySubObject("Cells(int,int)", row, moduleNumCell.column);
        QVariant value = cell->property("Value");
        if (value.isValid())
        {
            if (!value.toString().isEmpty())
            {
                moduleNum = value.toString();
                break;
            }
        }
        delete cell;
    }

    for (int row = moduleNameCell.row + 1; row <= rowCount; ++row)
    {
        QAxObject *cell = sheet->querySubObject("Cells(int,int)", row, moduleNameCell.column);
        QVariant value = cell->property("Value");
        if (value.isValid())
        {
            if (!value.toString().isEmpty())
            {
                moduleName = value.toString();
                break;
            }
        }
        delete cell;
    }

    int num = 0;
    for (int row = testPointNumCell.row + 1; row <= rowCount; ++row)
    {
        QString value = sheet->querySubObject("Cells(int,int)", row, testPointNumCell.column)->property("Value").toString();
        if (!value.isEmpty())
        {
            QStringList stringList;
            stringList << value;
            for (int i = testPointNumCell.column + 1; i < columnCount; ++i)
            {
                stringList << sheet->querySubObject("Cells(int,int)", row, i)->property("Value").toString();
            }
            testCase[num] = stringList;
            ++num;
        }
    }

    workbook->dynamicCall("Close()");
    workbooks->dynamicCall("Close()");
    excel.dynamicCall("Quit()");

    delete columns;
    delete rows;
    delete usedRange;
    delete sheet;
    delete sheets;
    delete workbook;
    delete workbooks;

    return testCase;
}

void ExcelScripter::updateTree()
{
    topoNum = 0;
    topoItemMap.clear();

    if (ui->comboBox_SelectFile->currentText().isEmpty())
    {
        ui->label_Tips->setText("请选择Excel文件!");
        QTimer::singleShot(1000, this, [this]()
                           { ui->label_Tips->setText(""); });
        return;
    }

    // 文件选择历史记录
    QStringList newFileHistory;
    fileHistory << ui->comboBox_SelectFile->currentText();

    for (int i = fileHistory.length() - 1; i >= 0; --i)
    {
        if (!newFileHistory.contains(fileHistory[i]))
        {
            newFileHistory.append(fileHistory[i]);
        }
    }

    fileHistory = newFileHistory;

    ui->comboBox_SelectFile->clear();
    ui->comboBox_SelectFile->addItems(fileHistory);

    ui->treeWidget->clear();

    QMap<int, QStringList> map = readExcel();
    if (map.isEmpty())
    {
        return;
    }
    ui->lineEdit_ModuleName->setText(moduleName);
    ui->lineEdit_ModuleNum->setText(moduleNum);

    int column = 0;
    for (const QString &title : headTitleList)
    {
        ui->treeWidget->headerItem()->setText(column, title);
        ++column;
    }

    QMap<int, QStringList>::const_iterator it;

    int testPointColumn = 0;
    for (int i = 0; i < ui->treeWidget->columnCount(); ++i)
    {
        if (ui->treeWidget->headerItem()->text(i).contains("说明"))
        {
            testPointColumn = i;
            break;
        }
    }
    int selectNum = 0;
    for (it = map.constBegin(); it != map.constEnd(); ++it)
    {
        QStringList infoList = it.value();
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
        int i = 0;
        for (const QString &string : infoList)
        {

            item->setText(i, string);
            if (i == testPointColumn)
            {
                if (string.isEmpty())
                {
                    item->setCheckState(0, Qt::Unchecked);
                }
                else
                {
                    item->setCheckState(0, Qt::Checked);
                    ++selectNum;
                }
            }

            ++i;
        }
    }

    ui->label_Tips2->setText("当前选中" + QString::number(selectNum) + "个用例");
    enableWidget();

    // 使用 data 保存每个 item各自的第一列的原文本，用于后期生成脚本
    QTreeWidgetItemIterator item(ui->treeWidget);
    while (*item)
    {
        (*item)->setData(column::ItemText, Qt::UserRole, (*item)->text(0));
        ++item;
    }

    // 将 headitem 设置为过滤选项
    ui->comboBox_FilterHeadItem->clear();
    titleColumnMap.clear();

    bool isFirstLoop = true;
    // 遍历头项
    for (int i = 0; i < ui->treeWidget->columnCount(); ++i)
    {
        QString headText = ui->treeWidget->headerItem()->text(i);
        ui->comboBox_FilterHeadItem->addItem(headText);
        titleColumnMap[headText] = i;
        if (headText.contains("Auto") && isFirstLoop)
        {
            QStringList textList;
            for (int j = 0; j < ui->treeWidget->topLevelItemCount(); ++j)
            {
                QString text = ui->treeWidget->topLevelItem(j)->text(i);
                if (!text.isEmpty())
                {
                    textList.append(text);
                }
            }
            // textList元素去重
            QSet<QString> uniqueSet;
            foreach (const QString &text, textList)
            {
                uniqueSet.insert(text);
            }

            ui->comboBox_FilterContent->addItems(uniqueSet.values());
            isFirstLoop = false;
        }
    }
    ui->comboBox_FilterContent->addItem("无");
    ui->comboBox_FilterContent->setCurrentText("无");
}

void ExcelScripter::createScript()
{
    if (ui->treeWidget->topLevelItemCount() == 0)
    {
        ui->label_Tips->setText("请先读取Excel文件!");
        QTimer::singleShot(1000, this, [this]()
                           { ui->label_Tips->setText(""); });
        return;
    }

    if (ui->lineEdit_Author->text().isEmpty())
    {
        ui->label_Tips->setText("缺少作者!");
        QTimer::singleShot(2000, this, [this]()
                           { ui->label_Tips->setText(""); });
        return;
    }

    if (ui->lineEdit_ModuleName->text().isEmpty())
    {
        ui->label_Tips->setText("缺少模块编号!");
        QTimer::singleShot(1000, this, [this]()
                           { ui->label_Tips->setText(""); });
        return;
    }

    if (ui->lineEdit_ModuleName->text().isEmpty())
    {
        ui->label_Tips->setText("缺少模块名!");
        QTimer::singleShot(1000, this, [this]()
                           { ui->label_Tips->setText(""); });
        return;
    }

    int checkedItemCount = topoItemMap.count();

    QProgressDialog *p = new QProgressDialog(this);
    p->setLabelText("遍历文件...");
    p->setWindowTitle("创建脚本");
    p->setWindowModality(Qt::WindowModal);
    p->setAutoClose(true);
    p->setAutoReset(true);
    p->autoReset();
    p->setMaximum(checkedItemCount);
    p->setMinimumDuration(0);
    p->setStyleSheet("QProgressBar {"
                     "    border-radius: 7px;"
                     "    border: 3px solid  #BADAFB;"
                     "    height: 20px;"
                     "    border-radius: 13px;"
                     "    text-align: center;"
                     "}"
                     "QProgressBar::chunk {"
                     "    background-color: #FAE63C;"
                     "    margin: 6px;" // 向内缩小涂色部分
                     "}");
    int count = 0;

    QString date = QDateTime::currentDateTime().toString("yyyy-MM-dd");

    bool isJOJOStyle;
    if (ui->comboBox_ScriptStyle->currentText() == "JOJO黄金之风")
    {
        isJOJOStyle = true;
    }
    else
    {
        isJOJOStyle = false;
    }

    // 创建 topo文件
    for (int i = 1; i <= topoNum; ++i)
    {
        QString topoText;
        QString setuptText = "<TESTCASE_BEGIN>\n    <TESTCASE_HEADER_BEGIN>\n";
        QString clearText = "<TESTCASE_BEGIN>\n    <TESTCASE_HEADER_BEGIN>\n";

        QString topoName = ui->lineEdit_ModuleName->text() + "_" + ui->lineEdit_ModuleNum->text() + "_" + QString::number(i) + ".topo";
        QString setupName = ui->lineEdit_ModuleName->text() + "_" + ui->lineEdit_ModuleNum->text() + "_setup_" + QString::number(i) + ".tcl";
        QString clearName = ui->lineEdit_ModuleName->text() + "_" + ui->lineEdit_ModuleNum->text() + "_clear_" + QString::number(i) + ".tcl";

        QString author = "        <AUTHOR>        \"" + ui->lineEdit_Author->text() + " " + date + "\"\n";
        QString source = "        <SOURCE>        \"" + topoName + "\"\n";

        QString scriptPath = QDir::currentPath() + "/" + topoName;
        QString setupFilePath = QDir::currentPath() + "/" + setupName;
        QString clearFilePath = QDir::currentPath() + "/" + clearName;

        topoText.append("<TOPOLOGY> name \"" + topoName + "\"\n");
        topoText.append("<TOPOLOGY> graph \"\"\n");
        topoText.append("<TOPOLOGY> description \"\"\n");
        topoText.append("<TOPOLOGY> setup \"\"\n");
        topoText.append("<TOPOLOGY> clear \"\"\n\n");
        topoText.append("<TESTCASE_DEVICE_MAP_BEGIN>\n");
        topoText.append("<TESTCASE_DEVICE_MAP_END>\n");

        setuptText.append("        <TITLE>         \"setup\"\n");
        setuptText.append("        <INDEX>         \"setup\"\n");
        setuptText.append("        <LEVEL>         \"3\"\n");
        setuptText.append("        <WEIGHT>        \"3\"\n");
        setuptText.append("        <MODULE>        \"ANY\"\n");
        setuptText.append("        <TYPE>          \"FUN\"\n");
        setuptText.append(author);
        setuptText.append("        <LIMITATION>    \"Comware\"\n");
        setuptText.append("        <VERSION>       \"1.0\"\n");
        setuptText.append("        <DESIGN>        \"in\"\n");
        setuptText.append(source);
        setuptText.append("    <TESTCASE_HEADER_END>\n");
        setuptText.append("    <TESTCASE_DEVICE_MAP_BEGIN>\n");
        setuptText.append("    <TESTCASE_DEVICE_MAP_END>\n");
        setuptText.append("    \n\n        # ========================= 变量设置 ========================= #\n");
        setuptText.append("    \n\n        # ========================= 配置清理 ========================= #\n");
        if (isJOJOStyle)
        {
            setuptText.append("            # 调用 ClearAllConfig 清理topo接口以及全局设备配置\n");
            setuptText.append("            ClearAllConfig\n");
        }

        setuptText.append("    \n\n        # ========================= 测试仪配置 ========================= #\n");
        setuptText.append("    \n\n        # ========================= 设备基础配置 ========================= #\n");
        setuptText.append("    \n\n        # ========================= 检查项 ========================= #\n");

        if (isJOJOStyle)
        {
            setuptText.append("    \n\n        # ========================= 处理所有Check项 ========================= #\n");
            setuptText.append("            # 调用 CheckFinish 对所有Check项进行汇总，返回整个脚本的通过情况\n");
            setuptText.append("            CheckFinish\n\n");
        }
        setuptText.append("<TESTCASE_END>\n");

        clearText.append("        <TITLE>         \"clear\"\n");
        clearText.append("        <INDEX>         \"clear\"\n");
        clearText.append("        <LEVEL>         \"3\"\n");
        clearText.append("        <WEIGHT>        \"3\"\n");
        clearText.append("        <MODULE>        \"ANY\"\n");
        clearText.append("        <TYPE>          \"FUN\"\n");
        clearText.append(author);
        clearText.append("        <LIMITATION>    \"Comware\"\n");
        clearText.append("        <VERSION>       \"1.0\"\n");
        clearText.append(source);
        clearText.append("    <TESTCASE_HEADER_END>\n");
        clearText.append("    <TESTCASE_DEVICE_MAP_BEGIN>\n");
        clearText.append("    <TESTCASE_DEVICE_MAP_END>\n");

        if (isJOJOStyle)
        {
            clearText.append("    \n\n        # ========================= 名空间删除 配置清理 ========================= #\n");
            clearText.append("            # 调用 ClearNamespaceImport 删除导入的CustomFunction名空间，同时清理接口、设备全局配置\n");
            clearText.append("            ClearNamespaceImport\n\n\n");
        }
        else
        {
            clearText.append("    \n\n        # ========================= 配置清理 ========================= #\n\n\n");
        }
        clearText.append("<TESTCASE_END>\n");

        fileIO.writeFile(scriptPath, topoText);
        fileIO.writeFile(setupFilePath, setuptText);
        fileIO.writeFile(clearFilePath, clearText);
    }

    QList<int> topoNumList = topoItemMap.keys();
    // 创建 脚本 文件
    for (const int &num : topoNumList)
    {
        QString topoNum = QString::number(num);

        for (const QTreeWidgetItem *item : topoItemMap[num])
        {

            QString scriptText = "<TESTCASE_BEGIN>\n    <TESTCASE_HEADER_BEGIN>\n";
            QString topoName = ui->lineEdit_ModuleName->text() + "_" + ui->lineEdit_ModuleNum->text() + "_" + topoNum + ".topo";
            QString testPointNum = item->data(column::ItemText, Qt::UserRole).toString();
            QString scriptPath = QDir::currentPath() + "/" + ui->lineEdit_ModuleName->text() + "_" + ui->lineEdit_ModuleNum->text() + "." + testPointNum + "_1_1_" + topoNum + ".tcl";

            scriptText.append("        <TITLE>         \"" + item->text(column::TestPointTitle) + "\"\n");
            scriptText.append("        <INDEX>         \"" + testPointNum + "\"\n");
            scriptText.append("        <LEVEL>         \"3\"\n");
            scriptText.append("        <WEIGHT>        \"3\"\n");
            scriptText.append("        <MODULE>        \"ANY\"\n");
            scriptText.append("        <TYPE>          \"FUN\"\n");
            scriptText.append("        <AUTHOR>        \"" + ui->lineEdit_Author->text() + " " + date + "\"\n");
            scriptText.append("        <LIMITATION>    \"Comware\"\n");
            scriptText.append("        <VERSION>       \"1.0\"\n");

            QString description = "\n";
            QString STEP;
            QStringList testPointList = item->text(column::Description).split('\n');
            int stepIdx = 1;

            scriptText.append("        <DESIGN>        \"" + description + "        \"\n");
            scriptText.append("        <SOURCE>        \"" + topoName + "\"\n");
            scriptText.append("    <TESTCASE_HEADER_END>\n");
            scriptText.append("    <TESTCASE_DEVICE_MAP_BEGIN>\n");
            scriptText.append("    <TESTCASE_DEVICE_MAP_END>\n");
            scriptText.append("        # ========================= 变量设置 ========================= #\n\n\n");
            scriptText.append("        # ========================= 测试仪配置 ========================= #\n\n\n");

            if (isJOJOStyle)
            {
                for (const QString &line : testPointList)
                {
                    description.append("                        " + line + "\n");
                    if (!line.contains("测试要点") && !line.isEmpty())
                    {
                        STEP.append("        # ========================= <STEP " + QString::number(stepIdx) + "> ========================= #\n");
                        STEP.append("            STEP \"" + line + "\"\n\n\n");
                        ++stepIdx;
                    }
                }
            }
            else
            {
                for (const QString &line : testPointList)
                {
                    description.append("                        " + line + "\n");
                    if (!line.contains("测试要点") && !line.isEmpty())
                    {
                        STEP.append("        # ========================= <STEP " + QString::number(stepIdx) + "> ========================= #\n");
                        STEP.append("            <STEP> \"" + line + "\" {\n\n\n            }\n\n\n");
                        ++stepIdx;
                    }
                }
            }

            scriptText.append(STEP);

            scriptText.append("        # ========================= 配置还原 ========================= #\n\n\n");
            if (isJOJOStyle)
            {
                scriptText.append("        # ========================= 处理所有Check项 ========================= #\n");
                scriptText.append("            # 调用 CheckFinish 对所有Check项进行汇总，返回整个脚本的通过情况\n");
                scriptText.append("            CheckFinish\n\n");
            }

            scriptText.append("<TESTCASE_END>\n");

            fileIO.writeFile(scriptPath, scriptText);
            ++count;
            p->setLabelText(scriptPath);
            p->setValue(count);
        }
    }
    delete p;
    p = nullptr;

    ui->label_Tips->setText("创建完成!");
}

void ExcelScripter::itemClicked(QTreeWidgetItem *item, int /*column*/)
{

    item->checkState(0) == Qt::Checked ? item->setCheckState(0, Qt::Unchecked) : item->setCheckState(0, Qt::Checked);
    ui->pushButton_SplitDone->setEnabled(true);

    QTreeWidgetItemIterator it(ui->treeWidget);
    int count = 0;
    while (*it)
    {
        if ((*it)->checkState(0) == Qt::Checked)
        {
            ++count;
        }
        ++it;
    }
    ui->label_Tips2->setText("当前选中" + QString::number(count) + "个用例");
}

// 通过topo结构对用例进行分类
void ExcelScripter::splitByTopo()
{
    ++topoNum;

    ui->pushButton_Create->setEnabled(false);
    ui->pushButton_SplitDone->show();

    QTreeWidgetItemIterator it(ui->treeWidget);
    int R = RGBList[usedRGBIdx][0];
    int G = RGBList[usedRGBIdx][1];
    int B = RGBList[usedRGBIdx][2];
    if (usedRGBIdx < sizeof(RGBList) / sizeof(RGBList[0]))
    {
        ++usedRGBIdx;
    }
    else
    {
        usedRGBIdx = 0;
    }
    while (*it)
    {
        if ((*it)->checkState(0) == Qt::Checked)
        {
            (*it)->setData(column::BelongsTopoNum, Qt::UserRole, topoNum);
            (*it)->setData(column::IsSetColor, Qt::UserRole, true);
            (*it)->setText(0, "[Topo" + QString::number(topoNum) + "] " + (*it)->text(0));
            for (int i = 0; i < (*it)->columnCount(); ++i)
            {
                (*it)->setBackground(i, QBrush(QColor(R, G, B)));
                (*it)->setCheckState(0, Qt::Unchecked);
            }
        }

        ++it;
    }
}

void ExcelScripter::splitDone()
{
    QTreeWidgetItemIterator it(ui->treeWidget);
    topoItemMap.clear();
    int count = 0;

    while (*it)
    {
        if ((*it)->data(column::IsSetColor, Qt::UserRole).toBool() && !(*it)->isHidden())
        {
            (*it)->setCheckState(0, Qt::Checked);
            int num = (*it)->data(column::BelongsTopoNum, Qt::UserRole).toInt();
            topoItemMap[num].append(*it);
            ++count;
        }
        ++it;
    }
    ui->label_Tips2->setText("共选中" + QString::number(count) + "个用例");
    ui->pushButton_Create->setEnabled(true);
    ui->pushButton_SplitDone->hide();
}

void ExcelScripter::selectAllItem()
{

    if (ui->pushButton_SelectAll->text() == "全选")
    {


        ui->pushButton_SelectAll->setIcon(QIcon(":/icon/icons8-circle-96.png"));
        ui->pushButton_SelectAll->setText("全不选");
        QTreeWidgetItemIterator it(ui->treeWidget);
        while (*it)
        {
            (*it)->setCheckState(0, Qt::Checked);
            ++it;
        }
    }
    else
    {
        ui->pushButton_SelectAll->setIcon(QIcon(":/icon/icons8-checkmark-96.png"));
        ui->pushButton_SelectAll->setText("全选");
        QTreeWidgetItemIterator it(ui->treeWidget);
        while (*it)
        {
            (*it)->setCheckState(0, Qt::Unchecked);
            ++it;
        }
    }
}

void ExcelScripter::enableWidget()
{
    ui->lineEdit_Author->setEnabled(true);
    ui->lineEdit_ModuleName->setEnabled(true);
    ui->lineEdit_ModuleNum->setEnabled(true);
    ui->comboBox_ExportPath->setEditable(true);
    ui->pushButton_SplitByTopo->setEnabled(true);
    ui->comboBox_ScriptStyle->setEnabled(true);
    ui->pushButton_SelectAll->setEnabled(true);
}

void ExcelScripter::disableWidget()
{
    ui->lineEdit_Author->setEnabled(false);
    ui->lineEdit_ModuleName->setEnabled(false);
    ui->lineEdit_ModuleNum->setEnabled(false);
    ui->comboBox_ExportPath->setEnabled(false);
    ui->pushButton_SplitByTopo->setEnabled(false);
    ui->pushButton_Create->setEnabled(false);
    ui->comboBox_ScriptStyle->setEnabled(false);
    ui->pushButton_SelectAll->setEnabled(false);
}

void ExcelScripter::titleComboBoxTextChanged(const QString &text)
{
    ui->comboBox_FilterContent->clear();
    ui->comboBox_FilterContent->addItem("无");
    ui->comboBox_FilterContent->setCurrentText("无");

    int column = titleColumnMap[text];
    QStringList textList;
    for (int j = 0; j < ui->treeWidget->topLevelItemCount(); ++j)
    {
        QString text = ui->treeWidget->topLevelItem(j)->text(column);
        if (!text.isEmpty())
        {
            textList.append(text);
        }
    }
    // textList元素去重
    QSet<QString> uniqueSet;
    foreach (const QString &text, textList)
    {
        uniqueSet.insert(text);
    }

    ui->comboBox_FilterContent->addItems(uniqueSet.values());
}

void ExcelScripter::filterComboBoxTextChanged(const QString &text)
{
    // 每次重新选择过滤时，还需要把之前隐藏的item给显示出来才行
    QTreeWidgetItemIterator it(ui->treeWidget);
    while (*it)
    {
        (*it)->setHidden(false);
        ++it;
    }

    if (text != "无")
    {        
        int column = titleColumnMap[ui->comboBox_FilterHeadItem->currentText()];
        QTreeWidgetItemIterator it(ui->treeWidget);
        while (*it)
        {
            if ((*it)->text(column) != text)
            {
                (*it)->setHidden(true);
            }
            ++it;
        }
    }
}
