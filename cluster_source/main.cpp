#include "cluster.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    cluster w;
    w.resize(635,230);
    w.show();

    return a.exec();
}
