#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QWidget>
#include <QPixmap>
#include <QPainter>
#include <QTimer>
#include <QDebug>
#include <QPushButton>
#include <QtNetwork/QTcpSocket>
#include <QRegExp>
#include <QStackedWidget>

namespace Ui {
class controller;
}

class controller : public QWidget
{
    Q_OBJECT



public:  QPixmap excel;
        QPixmap brake;
        QTcpSocket *socket;

private:

    QTimer *timer = new QTimer(this);
    QPushButton *btn[4];//P, R, N, D
    QPushButton *btn2[2];//break, excel
    int pressed;

public:
    explicit controller(QWidget *parent = nullptr, QTcpSocket *socket_in = 0);
    ~controller();

public:
    void paintEvent(QPaintEvent *event);

private slots:
    void brake_pressed();
    void accel_pressed();
    void brake_released();
    void accel_released();
    void P_click();
    void R_click();
    void N_click();
    void D_click();
    void connected();
    void color_changed(int what);

private:
    Ui::controller *ui;
};

#endif // CONTROLLER_H
