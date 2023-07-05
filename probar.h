#ifndef PROBAR_H
#define PROBAR_H

#include <QProgressDialog>
class Probar
{
public:
    Probar(QWidget *widget,int maxNum);
    ~Probar();
    void setLabelText(const QString&string);
private:
    QProgressDialog *p;
    int incrNum;
};

#endif // PROBAR_H
