#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QTranslator tr;
    tr.load(":/ja");
    QApplication a(argc, argv);
    a.installTranslator(&tr);
    MainWindow w;
    w.show();

    return a.exec();
}
