#include "mainwindow.h"
#include <QApplication>
#include <QFontDatabase>
#include <QDebug>
#include <QQuickStyle>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle(QStringLiteral("网易云音乐"));
    w.show();
    int localFont = QFontDatabase::addApplicationFont(":/HarmonyOS_Sans_SC_Regular.ttf");
    QString msyh = QFontDatabase::applicationFontFamilies(localFont).at(0);
    qDebug() << msyh;
    QFont font(msyh,10);
    font.setFamily(msyh);
    a.setFont(font);
    QFile file(":/style.qss");
    Q_ASSERT(file.open(QIODevice::ReadOnly));
    QByteArray dataSheet;
    while(!file.atEnd())
    {
        dataSheet +=  file.readLine();
    }
    a.setStyleSheet(dataSheet);
    file.close();
    return a.exec();
}
