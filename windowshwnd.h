#ifndef WINDOWSHWND_H
#define WINDOWSHWND_H

#include <QWidget>
#include <QPainter>

namespace Ui {
class WindowsHWND;
}

class WindowsHWND : public QWidget
{
    Q_OBJECT

public:
    explicit WindowsHWND(QWidget *parent = nullptr);
    ~WindowsHWND();
protected:
    void paintEvent(QPaintEvent *event);
private slots:
    void on_pushButton_clicked();

private:
    Ui::WindowsHWND *ui;
};

#endif // WINDOWSHWND_H
