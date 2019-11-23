#include "controller.h"
#include "ui_controller.h"

controller::controller(QWidget *parent,QTcpSocket *socket_in) : QWidget(parent),ui(new Ui::controller)
{

    pressed=1;
    //socket communication
//     socket = new QTcpSocket(this);
//     socket->connectToHost("192.168.100.136", 8000);
    socket = socket_in;


     connect(socket, SIGNAL(connected()), this,
                     SLOT(connected()));

       connect(timer, SIGNAL(timeout()), this, SLOT(update()));
       timer->start(10);

       excel.load(":/resources/excel.png","png");
       brake.load(":/resources/break.png","png");

       btn2[0]=new QPushButton(QString("Brake"),this);
       btn2[0]->setGeometry(125,250,80,40);
       btn2[1]=new QPushButton(QString("Accel"),this);
       btn2[1]->setGeometry(280,330,80,40);

       connect(btn2[0], SIGNAL(pressed()), this,
                       SLOT(brake_pressed()));
       connect(btn2[1], SIGNAL(pressed()), this,
                       SLOT(accel_pressed()));
       connect(btn2[0], SIGNAL(released()), this,
                       SLOT(brake_released()));
       connect(btn2[1], SIGNAL(released()), this,
                       SLOT(accel_released()));

       btn[0]=new QPushButton(QString("P"),this);
       btn[0]->setGeometry(450,80,40,40);
       btn[1]=new QPushButton(QString("R"),this);
       btn[1]->setGeometry(450,120,40,40);
       btn[2]=new QPushButton(QString("N"),this);
       btn[2]->setGeometry(450,160,40,40);
       btn[3]=new QPushButton(QString("D"),this);
       btn[3]->setGeometry(450,200,40,40);

       connect(btn[0], SIGNAL(clicked()), this,
               SLOT(P_click()));
       connect(btn[1], SIGNAL(clicked()), this,
               SLOT(R_click()));
       connect(btn[2], SIGNAL(clicked()), this,
               SLOT(N_click()));
       connect(btn[3], SIGNAL(clicked()), this,
               SLOT(D_click()));

       resize(600,400);

}

void controller::brake_pressed()
{
    qDebug("Brake pressed");
    QString message = "control=brakeon";
    socket->write(QString(message + "\0").toUtf8(),message.length()+1);
    btn2[0]->setStyleSheet("background-color:black;""color:white;");

}

void controller::accel_pressed()
{
    qDebug("Accel pressed");
    QString message = "control=accelon";
    socket->write(QString(message + "\0").toUtf8(),message.length()+1);
    btn2[1]->setStyleSheet("background-color:black;""color:white;");

}

void controller::brake_released()
{
    qDebug("Brake released");
    QString message = "control=brakeoff";
    socket->write(QString(message + "\0").toUtf8(),message.length()+1);
    btn2[0]->setStyleSheet("background-color:None;");
}

void controller::accel_released()
{
    qDebug("Accel released");
    QString message = "control=acceloff";
    socket->write(QString(message + "\0").toUtf8(),message.length()+1);
    btn2[1]->setStyleSheet("background-color:None;");
}


void controller::P_click()
{
    qDebug("P clicked");
    color_changed(0);
    QString message = "control=p";
    socket->write(QString(message + "\0").toUtf8(),message.length()+1);

}

void controller::R_click()
{
    qDebug("R clicked");
    color_changed(1);
    QString message = "control=r";
    socket->write(QString(message + "\0").toUtf8(),message.length()+1);
}

void controller::N_click()
{
    QString message = "control=n";
    color_changed(2);
    socket->write(QString(message + "\0").toUtf8(),message.length()+1);

    qDebug("N clicked");
}

void controller::D_click()
{
    QString message = "control=d";
    color_changed(3);
    socket->write(QString(message + "\0").toUtf8(),message.length()+1);

    qDebug("D clicked");
}

void controller::connected()
{
 QString message = "Control connected";
// socket->write(QString(message + "\0").toUtf8(),message.length()+1);
}

void controller::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter paint;
    paint.begin(this);
   // paint.scale(zoom, zoom);
    paint.drawPixmap(100,50, brake);
    paint.drawPixmap(250,40, excel);
    paint.end();

}


void controller::color_changed(int what){

    if(what==0){
        btn[0]->setStyleSheet("background-color:black;""color:white;");
        btn[1]->setStyleSheet("background-color:None;");
        btn[2]->setStyleSheet("background-color:None;");
        btn[3]->setStyleSheet("background-color:None;");
    }
    else if(what==1){
        btn[1]->setStyleSheet("background-color:black;""color:white;");
        btn[0]->setStyleSheet("background-color:None;");
        btn[2]->setStyleSheet("background-color:None;");
        btn[3]->setStyleSheet("background-color:None;");
    }
    else if(what==2){
        btn[2]->setStyleSheet("background-color:black;""color:white;");
        btn[0]->setStyleSheet("background-color:None;");
        btn[1]->setStyleSheet("background-color:None;");
        btn[3]->setStyleSheet("background-color:None;");
    }
    else if(what==3){
        btn[3]->setStyleSheet("background-color:black;""color:white;");
        btn[0]->setStyleSheet("background-color:None;");
        btn[1]->setStyleSheet("background-color:None;");
        btn[2]->setStyleSheet("background-color:None;");
    }

}
controller::~controller()
{
    delete ui;
}




