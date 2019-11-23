#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QWidget>
#include <QMediaPlayer>

class playlist : public QWidget
{
    Q_OBJECT
public:
    explicit playlist(QWidget *parent = 0);

//signals:

//public slots:

private:
    QMediaPlayer *player;
    QMediaPlaylist *playlistVar;
};

#endif // PLAYLIST_H
