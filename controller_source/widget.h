#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPixmap>
#include <QTcpSocket>
#include <QLabel>

class Widget : public QWidget
{
    Q_OBJECT
    QPixmap* pixmap;



public:
    Widget(QWidget *parent = 0,QTcpSocket *socket_in = 0);
    ~Widget();
    QLabel *result = new QLabel(this);

    QTcpSocket *socket;
    void paintEvent(QPaintEvent *);

private:


    bool on_off     = 0;

    bool left_up    = 0;
    bool left_down  = 0;

    bool play_stop  = 0;
    bool prev       = 0;
    bool next       = 0;
    bool vol_up     = 0;
    bool vol_down   = 0;

    bool emer       = 0;


private slots:
    void btn_click();
};

#endif // WIDGET_H
