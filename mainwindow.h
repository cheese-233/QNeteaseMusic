#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#if defined(_MSC_VER) && (_MSC_VER >= 1600)
# pragma execution_character_set("utf-8")
#endif
#include <QMainWindow>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QMediaPlayer>
#include <QDir>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSslSocket>
#include <QAbstractButton>
#include <QSignalMapper>
#include <QScrollBar>
#include <QPushButton>
#include <QMenu>
#include <QGraphicsPixmapItem>
#include <QGraphicsBlurEffect>
#include <QGraphicsScene>
#include <QPainter>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void playMusic();
    void getNetWork(const QList<QString> &strUrl);
    void searchPage();
    QList<long long int> *qPlayList;
    void addPlayList(int addId,bool isPlayNow = 0);
    void changeBanner(int i);
    void ResizePlayWidget(QSize size);
    void paintEvent(QPaintEvent*);
    QString LoginCookie = "";
    void addList(QJsonArray searchArray,QLayout *layout);
    void ChangeListShows();
private slots:
    void on_pushButton_clicked();
    void changePlayBar(qint64 value);
    void on_horizontalSlider_sliderMoved(int position);
    void finishPlay();
    void requestFinished(QNetworkReply* reply);
    void detailRequestFinished(QNetworkReply* reply);
    void picRequestFinished(QNetworkReply* reply);
    void searchRequestFinished(QNetworkReply* reply);
    void lrcRequestFinished(QNetworkReply* reply);
    void loginRequestFinished(QNetworkReply* reply);
    void listRequestFinished(QNetworkReply* reply);
    void on_pushButton_3_clicked();
    void on_pushBtn_clicked(int i);
    void on_lineEdit_returnPressed();
    void finishScroll(int value);
    void finishPageScroll(int value);
    void readGETthings(QNetworkReply* reply);
    void on_horizontalSlider_2_sliderMoved(int position);
    void nextPlaying(int i);
    void downloadMusic(int i);
    void on_pushButton_4_clicked();

    void on_toolButton_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_2_clicked();
    void stopWheel();
    void on_pushButton_7_clicked();
    void login_tab_changed(int i);
    void login_pushbutton_clicked();

    void on_tabWidget_2_currentChanged(int index);
    void on_list_clicked(int id);
    void on_PlayListPage_toolButton_clicked();
    void PlaylistRequestFinished(QNetworkReply* reply);
private:
    Ui::MainWindow *ui;
    void resizeEvent(QResizeEvent * event);
};
#endif // MAINWINDOW_H
