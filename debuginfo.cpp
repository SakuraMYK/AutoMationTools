#include "debuginfo.h"
#include "ui_debuginfo.h"

DebugInfo::DebugInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DebugInfo)
{
    ui->setupUi(this);
    ui->textEdit_Debug->setReadOnly(true);
    setWindowIcon(QIcon(":/icon/icons8-bug-64 (1).png"));

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
    QString timeString = "["+QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz")+ "] ";

//    QTextCursor cursor=ui->textEdit_Debug->textCursor();
//    cursor.setPosition(cursor.position(),QTextCursor::MoveAnchor);
//    qDebug() <<"first"<<cursor.position();

    ui->textEdit_Debug->append(timeString + text);

//    cursor.setPosition(cursor.position()+timeString.length(),QTextCursor::KeepAnchor);
//    cursor.select(QTextCursor::BlockUnderCursor);

//    qDebug() <<"second"<<cursor.position();
//    QTextCharFormat format;
//    format.setBackground(Qt::red);
//    cursor.mergeCharFormat(format);
//    cursor.clearSelection();
//    cursor.movePosition(QTextCursor::EndOfLine);
}


