#include "windowshwnd.h"
#include "ui_windowshwnd.h"
#include <QLabel>
WindowsHWND::WindowsHWND(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WindowsHWND)
{
//    ui->setupUi(this);
    this->resize(960,640);
//        this->setWindowFlag(Qt::FramelessWindowHint);   //设置无边框
}

WindowsHWND::~WindowsHWND()
{
    delete ui;
}

void WindowsHWND::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    //绘图前准备画笔、画刷
    QPen pen; //画笔。绘制图形边线，由颜色、宽度、线风格等参数组成
    pen.setColor(QColor(255,0,0,255));
    QBrush brush;   //画刷。填充几何图形的调色板，由颜色和填充风格组成
    brush.setColor(QColor(0,255,0,120));
    brush.setStyle(Qt::SolidPattern);
    QPainter painter(this);   //可在QPaintDevice上绘制各种图形。QPaintDevice有之类QWidget、QImage、QOpenGLPaintDevice等
    painter.setPen(pen);
    painter.setBrush(brush);
    painter.drawLine(QPoint(0,0),QPoint(this->rect().width()-50,0));   //QPainter绘制直线
    painter.drawRect(50,50,200,100);

    //绘制图片
    QPixmap pixmap;
    pixmap.load((QString(":/icon/icons8-bug-64 (1).png")));
    painter.drawPixmap(rect().width()-50,0,50,50,pixmap);

    qDebug()<<rand();

}

void WindowsHWND::on_pushButton_clicked()
{
    QPixmap pixmap(200, 200); // 创建一个大小为200x200的QPixmap
    pixmap.fill(Qt::transparent); // 填充透明背景

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QColor(0, 160, 230));

    painter.drawLine(QPointF(0, pixmap.height()), QPointF(pixmap.width() / 2, pixmap.height() / 2));

    QLabel* label = new QLabel(this);
    label->setPixmap(pixmap);
    label->show();
}

