#include "avn.h"
#include "ui_avn.h"

AVN::AVN(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AVN)
{
    socket = new QTcpSocket(this);
    socket->connectToHost("192.168.100.136", 8000);
    connect(socket,SIGNAL(readyRead()),this, SLOT(getData()));

    tb = new QLineEdit(this);//현재 재생곡 표시
    tb->setGeometry(280, 70, 220, 20);

    now = new QLabel(this);
    now->setGeometry(280,50,100,20);

      nextwhat=0;
      connect(timer, SIGNAL(timeout()), this, SLOT(update()));
      timer->start(10);
      music.load(":/resources/yeah.png","png");


      //움직이는 강아지
      movie = new QMovie(":/resources/doggy.gif","gif");
      movie->setScaledSize(QSize(230,230));
      QLabel *processLabel = new QLabel(this);
      processLabel->setMovie(movie);
      processLabel->setGeometry(50,40,250,250);
      movie->start();
      connect(movie, SIGNAL(finished()), this,
                      SLOT(keepmove()));



    //음악 재생, 이전곡, 다음곡 버튼 생성
      btn[0]=new QPushButton(QString("back"),this);
      btn[0]->setGeometry(140,270,80,40);
      btn[1]=new QPushButton(QString("play/stop"),this);
      btn[1]->setGeometry(240,270,80,40);
      btn[2]=new QPushButton(QString("next"),this);
      btn[2]->setGeometry(340,270,80,40);

      btn[0]->setStyleSheet("background-color:white;" "color:black;");
      btn[1]->setStyleSheet("background-color:white;" "color:black;");
      btn[2]->setStyleSheet("background-color:white;" "color:black;");


      //볼륨
          dial = new QDial(this);
          dial->setRange(0, 100);
          dial->setGeometry(450, 260, 80, 80);
          dial->setValue(50);
          vol2=50;

      //음악 리스트 띄우기
      textEdit = new QPlainTextEdit(this);
      textEdit->setGeometry(280, 100, 220, 150);
      QDir dir("C:\\musicList");
      playlist=dir.entryList(QStringList()<<"*mp3"<<"*.mp3",QDir::Files);

      for(int i=0; i<playlist.size();i++){
          musicCD.append(playlist[i]);
          musicCD.append("\n");
      }
      textEdit->setPlainText(musicCD);

      QString diroute="C:\\musicList\\";

      mp = new QMediaPlayer(this);
      mpl = new QMediaPlaylist();
      qDebug() << QFileInfo(QString("%1%2").arg(diroute).arg(playlist[0])).absoluteFilePath();
      qDebug() << QFileInfo(QString("%1%2").arg(diroute).arg(playlist[1])).absoluteFilePath();
      qDebug() << QFileInfo(QString("%1%2").arg(diroute).arg(playlist[2])).absoluteFilePath();
      mpl->addMedia(QUrl::fromLocalFile(QFileInfo(QString("%1%2").arg(diroute).arg(playlist[0])).absoluteFilePath()));
      mpl->addMedia(QUrl::fromLocalFile(QFileInfo(QString("%1%2").arg(diroute).arg(playlist[1])).absoluteFilePath()));
      mpl->addMedia(QUrl::fromLocalFile(QFileInfo(QString("%1%2").arg(diroute).arg(playlist[2])).absoluteFilePath()));
      mpl->addMedia(QUrl::fromLocalFile(QFileInfo(QString("%1%2").arg(diroute).arg(playlist[3])).absoluteFilePath()));

      mpv = new QVideoWidget(this);
      mpv->setAspectRatioMode(Qt::IgnoreAspectRatio);

      mp->setVolume(50);
      mp->setPlaybackRate(1);
      mp->setVideoOutput(mpv);

      mpl->setCurrentIndex(0);
      mp->setPlaylist(mpl);

        clicked=0;
        connect(btn[0], SIGNAL(clicked()), this,
                        SLOT(music_back()));
        connect(btn[1], SIGNAL(clicked()), this,
                        SLOT(music_play()));
        connect(btn[2], SIGNAL(clicked()), this,
                        SLOT(music_next()));
        resize(600,400);

}
void AVN::keepmove(){

      movie->start();
}

void AVN::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter paint;
    paint.begin(this);
    paint.drawPixmap(50,50,500,300, music);
    paint.drawPixmap(90,100,150,150, Baam);
    paint.end();

}

void AVN::changedData(int vol)
{
    qDebug("QDial 1 value : %d", vol);

    if(vol==1)
        vol2+=10;
    else if(vol==0)
        vol2-=10;

    if(vol2<0)
        vol2=0;
    if(vol2>100)
        vol2=100;

    qDebug("QDial 2 value : %d", vol2);
    //mp->setVolume(dial->value());
    mp->setVolume(vol2);
    dial->setValue(vol2);
}

void AVN::music_back()
{
    clicked=0;
    nextwhat--;
    if(nextwhat<0)
        nextwhat=playlist.size();

    mpl->setCurrentIndex(nextwhat);
    qDebug() << nextwhat << playlist.size();
    music_play();
}

void AVN::music_play()
{
    qDebug("play_pressed");
    tb->setText(playlist[nextwhat]);

    if(clicked==0){
        qDebug("clicked==0");
        now->setText("현재 재생중..");
        mp->play();
        clicked=1;
    }
    else if(clicked==1){
        qDebug("clicked==1");
        now->setText("멈춤..");
        mp->stop();
        clicked=0;
    }
}

void AVN::getData(){
    qDebug ("Doggy");
    while(socket->canReadLine()){
        info = QString::fromUtf8(socket->readLine()).trimmed();
        qDebug() << info;

        if(info=="handle=play")
            music_play();
        else if(info=="handle=previous")
            music_back();
        else if(info=="handle=next")
            music_next();
        else if(info=="handle=volumeup"){
            vol=1;
            changedData(vol);
        }
        else if(info=="handle=volumedown"){
            vol=0;
            changedData(vol);
        }
    }
}

void AVN::music_next()
{
    qDebug() << nextwhat << playlist.size();
    clicked=0;
    nextwhat++;
    if(nextwhat>playlist.size())
        nextwhat=0;

    mpl->setCurrentIndex(nextwhat);
    qDebug() << nextwhat << playlist.size();
    music_play();
}


AVN::~AVN()
{
    delete ui;
}
