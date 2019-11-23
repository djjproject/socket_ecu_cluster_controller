#include "controller.h"
#include "widget.h"
#include <QApplication>
#include <QTcpSocket>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTcpSocket *sock = new QTcpSocket();
    sock->connectToHost("192.168.100.136", 8000);

    controller w(0,sock);

    w.resize(400,300);
    w.setGeometry(750,500,600,400);
    w.show();

    Widget k(0,sock);


    k.resize(650,650);
    k.setGeometry(50,50,650,650);
    k.show();

    return a.exec();
}
