#include "debuginfo.h"
#include "ui_debuginfo.h"

DebugInfo::DebugInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DebugInfo)
{
    ui->setupUi(this);
}

DebugInfo::~DebugInfo()
{
    delete ui;
}


void DebugInfo::append(const QString & text)
{
    ui->textEdit_Debug->append(text);
}
