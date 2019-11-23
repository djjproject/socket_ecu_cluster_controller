#include "widget.h"
#include <QPainter>
#include <QDebug>
#include <QTimer>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QTcpSocket>
#include <QPixmap>

Widget::Widget(QWidget *parent,QTcpSocket *socket_in):QWidget(parent)
{
    QTimer *timer  = new QTimer(this);
    connect(timer,SIGNAL(timeout()), this, SLOT(update()));
    timer->start(10);

    pixmap = new QPixmap();
    pixmap->load(":/resources/handle.png","png");
  // {
   //     qDebug() << "Fatal error: Unable to load image";
   //     exit(-1);
   // }

//    socket = new QTcpSocket(this);
//    socket->connectToHost("192.168.100.136", 8000);

    socket = socket_in;

    QPushButton *btn[9];
    btn[0] = new QPushButton(QString("Next"), this);
    btn[0]->setGeometry(20,250,100,50);

    btn[1] = new QPushButton(QString("Play/Stop"), this);
    btn[1]->setGeometry(20,300,100,50);

    btn[2] = new QPushButton(QString("Previous"), this);
    btn[2]->setGeometry(20,350,100,50);

    btn[3] = new QPushButton(QString("Volume Up"), this);
    btn[3]->setGeometry(530,275,100,50);

    btn[4] = new QPushButton(QString("Volume Down"), this);
    btn[4]->setGeometry(530,325,100,50);

    btn[5] = new QPushButton(QString("On/Off"), this);
    btn[5]->setGeometry(270,90,100,50);

    btn[6] = new QPushButton(QString("Left"), this);
    btn[6]->setGeometry(90,90,100,50);

    btn[7] = new QPushButton(QString("Right"), this);
    btn[7]->setGeometry(450,90,100,50);

    btn[8] = new QPushButton(QString("Emergency"), this);
    btn[8]->setGeometry(275,390,100,50);

    connect(btn[0], SIGNAL(clicked()), this, SLOT(btn_click()));
    connect(btn[1], SIGNAL(clicked()), this, SLOT(btn_click()));
    connect(btn[2], SIGNAL(clicked()), this, SLOT(btn_click()));
    connect(btn[3], SIGNAL(clicked()), this, SLOT(btn_click()));
    connect(btn[4], SIGNAL(clicked()), this, SLOT(btn_click()));
    connect(btn[5], SIGNAL(clicked()), this, SLOT(btn_click()));
    connect(btn[6], SIGNAL(clicked()), this, SLOT(btn_click()));
    connect(btn[7], SIGNAL(clicked()), this, SLOT(btn_click()));
    connect(btn[8], SIGNAL(clicked()), this, SLOT(btn_click()));
}

void Widget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter paint;
    paint.begin(this);
    paint.drawPixmap(115,100, *pixmap);
    paint.end();
}

void Widget::btn_click(){

    QPushButton* pButton = qobject_cast<QPushButton*>(sender());

    QString input = pButton->text();

    if(pButton){
        if(input.contains("Next", Qt::CaseInsensitive)){
            //on_off = true;
            QString message = "handle=next";
            socket->write(QString(message + "\0").toUtf8(),message.length()+1);
        }
        if(input.contains("Play/Stop", Qt::CaseInsensitive)){
            //play_stop = true;
            QString message = "handle=play";
            socket->write(QString(message + "\0").toUtf8(),message.length()+1);
        }
        if(input.contains("Previous", Qt::CaseInsensitive)){
            //prev = true;
            QString message = "handle=previous";
            socket->write(QString(message + "\0").toUtf8(),message.length()+1);
        }
        if(input.contains("Volume Up", Qt::CaseInsensitive)){
            //vol_up = true;
            QString message = "handle=volumeup";
            socket->write(QString(message + "\0").toUtf8(),message.length()+1);
        }
        if(input.contains("Volume Down", Qt::CaseInsensitive)){
            //vol_down = true;
            QString message = "handle=volumedown";
            socket->write(QString(message + "\0").toUtf8(),message.length()+1);
        }
        if(input.contains("On/Off", Qt::CaseInsensitive)){
            //on_off = true;
            QString message = "handle=onoff";
            socket->write(QString(message + "\0").toUtf8(),message.length()+1);
        }
        if(input.contains("Left", Qt::CaseInsensitive)){
            //left_down = true;
            QString message = "handle=left";
            socket->write(QString(message + "\0").toUtf8(),message.length()+1);
        }
        if(input.contains("Right", Qt::CaseInsensitive)){
            //left_up = true;
            QString message = "handle=right";
            socket->write(QString(message + "\0").toUtf8(),message.length()+1);
        }
        if(input.contains("Emergency", Qt::CaseInsensitive)){
            //emer = true;
            QString message = "handle=emergency";
            socket->write(QString(message + "\0").toUtf8(),message.length()+1);
        }
    }
}

Widget::~Widget()
{

}
