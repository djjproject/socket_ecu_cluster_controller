#ifndef AVN_H
#define AVN_H

#include <QWidget>
#include <QPixmap>
#include <QPainter>
#include <QTimer>
#include <QDebug>
#include <QPushButton>
#include <QtNetwork/QTcpSocket>
#include <QtMultimedia>
#include <QVideoWidget>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QDial>
#include <QLabel>
#include <QLineEdit>

namespace Ui {
class AVN;
}

class AVN : public QWidget
{
    Q_OBJECT    

public:
    explicit AVN(QWidget *parent = nullptr);
    ~AVN();

public:  QPixmap music;
         QPixmap bg;
         QPixmap Baam;
         QMediaPlayer *mp;
         QMediaPlaylist *mpl;//playlist
         QVideoWidget *mpv;
         QPushButton *btn[3];//이전곡, 재생/일시정지, 다음곡
         QPushButton *volume[2];//볼륨
         QPushButton *openbtn;//파일열기
         int clicked;
         QPlainTextEdit *textEdit;//playlist 띄우기
         QStringList playlist;
         QString musicCD;
         int nextwhat;
         QDial* dial;
         QMovie *movie;
         QLabel *processLabel;
         QString info;//값 from cluster
         int vol;
         int vol2;
         QLineEdit *tb;
         QLabel* now;


private:
    QTcpSocket *socket;
    QTimer *timer = new QTimer(this);

private slots:
    void music_back();
    void music_play();
    void music_next();
    void changedData(int val);
    void keepmove();
    void getData();


public:
    void paintEvent(QPaintEvent *event);


private:
    Ui::AVN *ui;
};

#endif // AVN_H
