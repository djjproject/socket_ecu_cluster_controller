#include "cluster.h"
#include <QPainter>
#include <QDebug>
#include <QTimer>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QTcpSocket>
#include <QTime>

cluster::cluster(QWidget *parent)
    : QWidget(parent)
{

    QTimer *timer  = new QTimer(this);

    bLArrow = false;
    bRArrow = false;
    eng_tmp = 0;
    oil = 0;
    dist_all = 0;
    dist_cur = 0;

    socket = new QTcpSocket(this);
    socket->connectToHost("192.168.100.136", 8000);

    connect(timer,SIGNAL(timeout()), this, SLOT(update()));
    connect(socket,SIGNAL(readyRead()),this, SLOT(getData()));
    
    timer->start(20);

    //클러스터 배경화면 띄우기
    if(!pixmap_cluster.load(":/resources/cluster.png","png"))
    {
        qDebug() << "Fatal error: Unable to load image";
        exit(-1);
    }
    //좌측 방향등
    if(!pixmap_left.load(":/resources/left.png","png"))
    {
        qDebug() << "Fatal error: Unable to load image";
        exit(-1);
    }
    //우측 방향등
    if(!pixmap_right.load(":/resources/right.png","png"))
    {
        qDebug() << "Fatal error: Unable to load image";
        exit(-1);
    }

    //남은 기름
    progress[0] = new QProgressBar(this);
    progress[0]->setMinimum(0);
    progress[0]->setMaximum(100);
    progress[0]->setValue(50);
    progress[0]->setOrientation(Qt::Vertical);
    progress[0]->move(610,105);

    //엔진 온도
    progress[1] = new QProgressBar(this);
    progress[1]->setMinimum(0);
    progress[1]->setMaximum(100);
    //progress[1]->setValue(eng_tmp);
    progress[1]->setOrientation(Qt::Vertical);
    progress[1]->move(10,105);

    //누적 주행거리 표시
    lcd[0] = new QLCDNumber(this);
    lcd[0]->setSegmentStyle(QLCDNumber::Outline);
    lcd[0]->setFrameStyle(0);
    //lcd[0]->display(dist_all); //static 변수
    lcd[0]->setGeometry(250,190,50,30);

    //현재 주행거리 표시
    lcd[1] = new QLCDNumber(this);
    lcd[1]->setSegmentStyle(QLCDNumber::Outline);
    lcd[1]->setFrameStyle(0);
    //lcd[1]->display(dist_cur); //static 변수
    lcd[1]->setGeometry(340,190,50,30);

    //기어 상태 표시등
    QString str[4] = {"D", "N", "R", "P"};
    int xpos = 250;
    for(int i = 0 ; i < 4 ; i++)
    {
        gear[i] = new QRadioButton(str[i], this);
        gear[i]->setGeometry(xpos, 10, 40, 30);
        gear[i]->setStyleSheet("background-color:black;""color:red;");

        xpos += 40;
    }
}

void cluster::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    paint.begin(this);
    paint.drawPixmap(0,0, pixmap_cluster);

    paint.setRenderHint(QPainter::Antialiasing);
    QColor speedColor(255, 0, 0);
    paint.setBrush(speedColor);

    cnt++;

    if((cnt % 200) < 100){
        if(bLArrow == true){
            paint.drawPixmap(250,40, pixmap_left);
            qDebug() << info;
        }
        if(bRArrow == true){
            paint.drawPixmap(360,40, pixmap_right);
        }
    }

    else if((cnt % 200) > 100){
        qDebug() << info;
        bLArrow = false;
        bRArrow = false;
        if(cnt == 500) cnt = 0;
    }

    static const QPoint rpmPin[3] = {
        QPoint(7, 8),
        QPoint(-7, 8),
        QPoint(0, 70)
    };

    static const QPoint speedPin[3] = {
        QPoint(7, 8),
        QPoint(-7, 8),
        QPoint(0, 70)
    };

    paint.save();
    paint.translate(172.5,125);
    if(speed_angle < 40) paint.rotate(40);
    else if(speed_angle > 315) paint.rotate(315);
    else paint.rotate(speed_angle);
    paint.drawConvexPolygon(speedPin, 3);
    paint.restore();


    QColor rpmColor(255, 0, 0);
    paint.setBrush(rpmColor);


    paint.translate(468.5,125);

    paint.save();
    if(rpm_angle < 40) paint.rotate(40);
    else if(rpm_angle > 315) paint.rotate(315);
    else paint.rotate(rpm_angle);
    paint.drawConvexPolygon(rpmPin, 3);
    paint.restore();

    paint.end();
}

void cluster::getData(){
    while(socket->canReadLine()){
        info = QString::fromUtf8(socket->readLine()).trimmed();

        qDebug() << "START\n";
        if(info.contains("oil")){
            oil_left();
        }
        else if(info.contains("speed")){
            rpm_angle = ((info.split("="))[1].toInt()) * 1.7 + 38.3;
        }
        else if(info.contains("rpm")){
            speed_angle = ((info.split("="))[1].toInt()) * 3.5 + 36.5;
        }
        else if(info.contains("cel")){
            engtmp();
        }
        else if(info.contains("curodo")){
            speed();
        }
        else if(info.contains("odo")){
            speed_all();
        }
        else if(info.contains("control")){
            chgear();
        }        
        else if(info.contains("left")){
            bLArrow = true;
            bRArrow = false;
        }
        else if(info.contains("right")){
            bLArrow = false;
            bRArrow = true;
        }
        else if(info.contains("emer")){
            bLArrow = true;
            bRArrow = true;
        }
        qDebug() << "END\n";
    }
}

void cluster::speed(){
    if(info.contains("curodo")){
        qDebug() << ((info.split("="))[0]);
        qDebug() << ((info.split("="))[1].toInt());
        dist_cur = ((info.split("="))[1].toInt());
    }
    qDebug() << dist_cur << "dispcur\n";
    lcd[0]->display(dist_cur);

}

void cluster::speed_all(){
    if(info.contains("odo")){
        qDebug() << ((info.split("="))[0]);
        qDebug() << ((info.split("="))[1].toInt());
        dist_all = ((info.split("="))[1].toInt());
    }
    qDebug() << dist_all << "dispall\n";
    lcd[1]->display(dist_all);
}

void cluster::oil_left(){
    if(info.contains("oil")){
        qDebug() << ((info.split("="))[0]);
        qDebug() << ((info.split("="))[1].toInt());
        oil = ((info.split("="))[1].toInt()) * 10;
    }
    qDebug() << oil << "oil\n";
    progress[0]->setValue(oil);
}

void cluster::engtmp(){
    if(info.contains("cel")){
        eng_tmp = ((info.split("="))[1].toInt()) * 10;
    }
    qDebug() << eng_tmp << "eng_tmp\n";
    progress[1]->setValue(eng_tmp);
}

void cluster::chgear(){

    if(info.contains("control=d")){
        gear[0]->setChecked(true);
    }
    else if(info.contains("control=n"))
        gear[1]->setChecked(true);
    else if(info.contains("control=r"))
        gear[2]->setChecked(true);
    else if(info.contains("control=p"))
        gear[3]->setChecked(true);
}

cluster::~cluster()
{

}
