#include "debuginfo.h"
#include "ui_debuginfo.h"

DebugInfo::DebugInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DebugInfo)
{
    ui->setupUi(this);
    ui->textEdit_Debug->setReadOnly(true);

//    dialog = nullptr;
//    QShortcut *shorcut = new QShortcut(QKeySequence(Qt::CTRL|Qt::Key_F),this);
//    connect(shorcut,&QShortcut::activated,this,&DebugInfo::showDialog);

}

DebugInfo::~DebugInfo()
{
    delete ui;
}


void DebugInfo::append(const QString & text)
{
    ui->textEdit_Debug->append("["+QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz")+ "] " + text);
}


