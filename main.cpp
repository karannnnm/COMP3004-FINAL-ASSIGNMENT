// Devanjali Das, Gurleen Bassali, Karan Modi, Zainab Jahangir

#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
