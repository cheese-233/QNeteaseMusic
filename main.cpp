#include "mainwindow.h"

#include <QApplication>
#include <QFontDatabase>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("Õ¯“◊‘∆“Ù¿÷");
    w.show();
    int localFont = QFontDatabase::addApplicationFont(":/HarmonyOS_Sans_SC_Regular.ttf");
    QString msyh = QFontDatabase::applicationFontFamilies(localFont).at(0);
    QFont font(msyh,10);
    font.setFamily(msyh);
    QApplication::setFont(font);
    return a.exec();
}
