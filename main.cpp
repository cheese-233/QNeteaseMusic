#include "mainwindow.h"
#pragma execution_character_set("UTF-8")
#include <QApplication>
#include <QFontDatabase>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle(QString::fromUtf16(u"Õ¯“◊‘∆“Ù¿÷"));
    w.show();
    int localFont = QFontDatabase::addApplicationFont(":/HarmonyOS_Sans_SC_Regular.ttf");
    QString msyh = QFontDatabase::applicationFontFamilies(localFont).at(0);
    QFont font(msyh,10);
    font.setFamily(msyh);
    QApplication::setFont(font);
    return a.exec();
}
