#include <QDebug>
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    if(argc == 2)
    {
        QString fileName(argv[1]);
        w.openFile(fileName);
    }
    w.show();

    return a.exec();
}
