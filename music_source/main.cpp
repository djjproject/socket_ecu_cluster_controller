#include "avn.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AVN w;
    w.show();

    return a.exec();
}
