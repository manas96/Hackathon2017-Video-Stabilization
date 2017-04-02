#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QTextStream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFile file("style.qss");
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream stream(&file);
        a.setStyleSheet(stream.readAll());
        file.close();
    }
    MainWindow w;
    w.show();

    return a.exec();
}
