#include "probar.h"

Probar::Probar(QWidget *widget,int maxNum)
{

    // 创建一个进度条
    QProgressDialog *p = new QProgressDialog(widget);
    p->setCancelButtonText("Cancel");
    p->setLabelText("遍历文件...");
    p->setWindowModality(Qt::WindowModal);
    p->setAutoClose(true);
    p->setAutoReset(true);
    p->autoReset();
    p->setMaximum(maxNum);
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
}

Probar::~Probar()
{
    delete p;
}

void Probar::setLabelText(const QString &string)
{
    ++incrNum;
    p->setLabelText(string);
    p->setValue(incrNum);
}
