#include "dialog.h"
#include "login.h"
#include <QApplication>
#include <QPixmap>
#include <QSplashScreen>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    QPixmap pixmap("start.jpg");
//    QSplashScreen splash(pixmap);
//    splash.show();
//    a.processEvents();
//    for(long index=0;index<1000000000;index++);

    Login w1;
    w1.show();
    //splash.finish(&w);

    return a.exec();
}
