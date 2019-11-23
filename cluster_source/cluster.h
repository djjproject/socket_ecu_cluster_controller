#ifndef CLUSTER_H
#define CLUSTER_H

#include <QWidget>
#include <QPixmap>
#include <QTcpSocket>
#include <QLabel>
#include <QRadioButton>
#include <QButtonGroup>
#include <QtNetwork/QTcpSocket>
#include <QPainter>
#include <QProgressBar>
#include <QLCDNumber>


static int cnt = 0;

class cluster : public QWidget
{
    Q_OBJECT
    QPixmap pixmap_cluster;
    QPixmap pixmap_left;
    QPixmap pixmap_right;

public:
    cluster(QWidget *parent = 0);
    ~cluster();

    double speed_angle = 0;
    double rpm_angle = 0;
    bool arrow_flag = false;
    QPainter paint;
    QProgressBar *progress[2];
    QLCDNumber *lcd[2];
    QString info;

    int eng_tmp;
    int oil;
    int dist_all;
    int dist_cur;

    bool bLArrow;
    bool bRArrow;

    void paintEvent(QPaintEvent *);

private:
    QTcpSocket *socket;
    QRadioButton *gear[4];

    bool left_flag = false;
    bool right_flag = false;

public slots:
    void chgear();
    void speed();
    void speed_all();
    void oil_left();
    void engtmp();
    void getData();
    //void leftArrow();
    //void rightArrow();
    //void emer();
};

#endif // CLUSTER_H
