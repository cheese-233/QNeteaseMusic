#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_form.h"
#include <QThread>
#include <QLabel>
#include <QRandomGenerator>
#include <QBitmap>
#include <QPropertyAnimation>
#include <QTimer>
#include <QMessageBox>
#include <QtNetwork>
#include "qmusictab.h"
#include "qplaying.h"
#include "ui_qplaying.h"
#include "neteaselogin.h"
#include "ui_neteaselogin.h"
#include "neteaseplaylist.h"
#include "ui_neteaseplaylist.h"
QString localhostName;
QString NeteaseServer = "https://neteasecloudmusic.api.cheese233.cn.eu.org/";
NetEaseLogin::NetEaseLogin(QWidget *parent)
    :QWidget{parent}
{

}
NeteasePlayList::NeteasePlayList(QWidget *parent)
    :QWidget{parent}
{

}
void NeteasePlayList::mousePressEvent(QMouseEvent *ev)
{
    mousePos = QPoint(ev->x(), ev->y());
}

void NeteasePlayList::mouseReleaseEvent(QMouseEvent *ev)
{
    if(mousePos == QPoint(ev->x(), ev->y())) emit clicked();
}
QWidget *QPlayingWeiget;
QPlaying::QPlaying(QWidget *parent)
    :QWidget{parent}
{
    QPlayingWeiget = this;
}
QByteArray PlayingImage;
Ui::QPlaying *playUi;
QIcon TempBtn2Icon;
void changePlayingMask(){
    QPixmap pix;
    pix.QPixmap::loadFromData(PlayingImage);
    pix.scaled(playUi->label->size() ,Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QBitmap mask(playUi->label->size());
    QPainter painter(&mask);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.fillRect(0, 0, playUi->label->size().width(), playUi->label->size().height(), Qt::white);
    painter.setBrush(QColor(0, 0, 0));
    painter.drawRoundedRect(0, 0, playUi->label->size().width(), playUi->label->size().height(), 10, 10);
    QPixmap image = pix.scaled(playUi->label->size());
    image.setMask(mask);
    playUi->label->setPixmap(image);
    playUi->label->setScaledContents(true);
    playUi->label->setStyleSheet("border:0px;border-radius:10px;");
}
QImage applyEffectToImage(QImage src, QGraphicsEffect *effect, int extent = 0)
{
    if(src.isNull()) return QImage();   //No need to do anything else!
    if(!effect) return src;             //No need to do anything else!
    QGraphicsScene scene;
    QGraphicsPixmapItem item;
    item.setPixmap(QPixmap::fromImage(src));
    item.setGraphicsEffect(effect);
    scene.addItem(&item);
    QImage res(src.size()+QSize(extent*2, extent*2), QImage::Format_ARGB32);
    res.fill(Qt::transparent);
    QPainter ptr(&res);
    scene.render(&ptr, QRectF(), QRectF( -extent, -extent, src.width()+extent*2, src.height()+extent*2 ) );
    return res;
}
QColor pixmapMainColor(QImage image, double bright = 1)
{
    int step=20;
    int t=0;
    int r=0,g=0,b=0;
    for (int i=0;i<image.width();i+=step) {
        for (int j=0;j<image.height();j+=step) {
            if(image.valid(i,j)){
                t++;
                QColor c=image.pixel(i,j);
                r+=c.red();
                b+=c.blue();
                g+=c.green();
            }
        }
    }
        return QColor(int(bright*r/t)>255?255:int(bright*r/t),
                  int(bright*g/t)>255?255:int(bright*g/t),
                  int(bright*b/t)>255?255:int(bright*b/t));
}
QColor playerMainColor;
QImage playerBgImage;
void MainWindow::paintEvent(QPaintEvent*){
    if(ui->label->isHidden()){
        QPainter pp(this);
        QRect Temp(-50,-50,this->width()+100,this->height()+100);
        pp.drawImage(Temp,playerBgImage);
    }
}
QMainWindow *WhatsMain;
QMusicTab::QMusicTab(QWidget *parent)
    : QWidget{parent}
{
}
void QMusicTab::mousePressEvent(QMouseEvent *ev)
{
    mousePos = QPoint(ev->x(), ev->y());
}

void QMusicTab::mouseReleaseEvent(QMouseEvent *ev)
{
    if(mousePos == QPoint(ev->x(), ev->y())) emit clicked();
}
QMediaPlayer *player = new QMediaPlayer;
QUrl playUrl;
int playUid;
int isRange = 1;   //-1:random,0:range,1:signalRange
QString formatTime(int ms)
{
    int ss = 1000;
    int mi = ss * 60;


    long minute = ms / mi;
    long second = (ms - minute * mi) / ss;
    QString min;
    QString sec;
    if(minute < 10){
        min = "0"+QString::number(minute,10);
    }
    else{
        min = QString::number(minute,10);
    }
    if(second < 10){
        sec = "0"+QString::number(second,10);
    }
    else{
        sec = QString::number(second,10);
    }
    return min + ":" + sec ;
}
QJsonArray HomeBanner;
QSettings settings(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationName(), "QNeteaseMusic");
int HomeBannerNow = 0;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    QList<QHostAddress> list =QNetworkInterface::allAddresses();
    foreach (QHostAddress address, list)
    {
       if(address.protocol() ==QAbstractSocket::IPv4Protocol)
           localhostName = address.toString();
    }
    if(localhostName.isEmpty()){
        localhostName = "127.0.0.1";
    }
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/favicon.ico"));
    ui->pushButton->setIcon(QIcon(":/stop.png"));
    connect(player,SIGNAL(positionChanged(qint64)),this,SLOT(changePlayBar(qint64)));
    player->setVolume(100);
    qPlayList = new QList<int>();
    HomeBanner = QJsonDocument::fromJson(Qget(NeteaseServer+"/banner")).object().value("banners").toArray();
    changeBanner(HomeBannerNow);
    WhatsMain = this;
    LoginCookie = settings.value("Cookie","").toString();
    QJsonObject LoginProfile = settings.value("profile").toJsonObject();
    if(!LoginProfile.isEmpty()){
        ui->label_9->setText(LoginProfile.value("nickname").toString());
    }
}
void MainWindow::changeBanner(int i){
    QString imageStr = "http://"+QUrl(HomeBanner[i].toObject().value("imageUrl").toString()).host()+QUrl(HomeBanner[i].toObject().value("imageUrl").toString()).path()+"?param=680y200";
    qDebug() << imageStr;
    QPixmap picmap;
    picmap.QPixmap::loadFromData(Qget(imageStr));
    ui->label_7->setPixmap(picmap);
    ui->label_6->setText(HomeBanner[i].toObject().value("typeTitle").toString());
}
bool isNotPlayOver = 0;
void MainWindow::playMusic(){
    player->stop();
    isNotPlayOver = 1;
    ui->pushButton->setIcon(QIcon(":/stop.png"));
    if(!QJsonDocument::fromJson(Qget(NeteaseServer+"check/music?id="+QString::number(playUid)+"&cookie="+LoginCookie.toUtf8().toPercentEncoding())).object().value("success").toBool()){
        qPlayList->removeOne(playUid);
        playUid = NULL;
        return;
    }
    QNetworkRequest request;
    QNetworkAccessManager* naManager = new QNetworkAccessManager(this);
    QMetaObject::Connection connRet = QObject::connect(naManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(requestFinished(QNetworkReply*)));
    Q_ASSERT(connRet);
    request.setUrl(QUrl(NeteaseServer+"song/url?id="+QString::number(playUid)+"&cookie="+LoginCookie.toUtf8().toPercentEncoding()));
    naManager->get(request);
    QNetworkRequest detailRequest;
    QNetworkAccessManager* detailNaManager = new QNetworkAccessManager(this);
    QMetaObject::Connection detailConnRet = QObject::connect(detailNaManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(detailRequestFinished(QNetworkReply*)));
    Q_ASSERT(detailConnRet);
    detailRequest.setUrl(QUrl(NeteaseServer+"song/detail?ids="+QString::number(playUid)+"&cookie="+LoginCookie.toUtf8().toPercentEncoding()));
    detailNaManager->get(detailRequest);
}
void MainWindow::detailRequestFinished(QNetworkReply* reply){
    QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if(statusCode.isValid())
        qDebug() << "status code=" << statusCode.toInt();

    QVariant reason = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
    if(reason.isValid())
        qDebug() << "reason=" << reason.toString();

    QNetworkReply::NetworkError err = reply->error();
    if(err != QNetworkReply::NoError) {
        qFatal("Falied to read Music Detail");
    }
    else {
        QJsonDocument document = QJsonDocument::fromJson(reply->readAll());
        if (!document.isNull()){
            if(document.isObject()){
                QJsonObject object = document.object();
                if(object.contains("songs")){
                    QJsonValue value = object.value("songs");
                    QJsonArray dataArray = value.toArray();
                    QJsonObject dataObject = dataArray[0].toObject();
                    if(dataObject.contains("name")){
                        QString songName = dataObject.value("name").toString();
                        ui->label_2->setText(songName);
                        if(ui->label_2->isHidden()){
                            playUi->label_2->setText(songName);
                        }
                    }
                    if(dataObject.contains("ar")){
                        QString nameString = dataObject.value("ar").toArray()[0].toObject().value("name").toString();
                        ui->label->setText(nameString);
                        if(ui->label->isHidden()){
                            playUi->label_3->setText(nameString);
                        }
                    }

                    if(dataObject.contains("al")){
                        QString picString = dataObject.value("al").toObject().value("picUrl").toString();
                        picString = "http://"+QUrl(picString).host()+QUrl(picString).path();
                        qDebug() << picString;
                        QNetworkRequest picRequest;
                        QNetworkAccessManager* picNaManager = new QNetworkAccessManager(this);
                        QMetaObject::Connection picConnRet = QObject::connect(picNaManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(picRequestFinished(QNetworkReply*)));
                        Q_ASSERT(picConnRet);
                        picRequest.setUrl(QUrl(picString));
                        picNaManager->get(picRequest);
                    }

                }
            }
        }
    }
}
QList<int> listLyricsTime;
QList<QString> listLyricsText;
QMap<int,QString> listTLyrics;
void ChangePlayerMainColor(){
    playerMainColor = pixmapMainColor(QImage::fromData(PlayingImage));
    if(playerMainColor.red()+playerMainColor.blue()+playerMainColor.green() > 127*3){
        QPlayingWeiget->setStyleSheet("QLabel{color:rgb(0,0,0);}QLabel[rgba = \"true\"]{color:rgba(0,0,0,200);}");
    }
    else{
        QPlayingWeiget->setStyleSheet("QLabel{color:rgb(255,255,255);}QLabel[rgba = \"true\"]{color:rgba(255,255,255,200);}");
    }

    playerBgImage.QImage::loadFromData(PlayingImage);
    QGraphicsBlurEffect *b_effect = new QGraphicsBlurEffect();
    b_effect->setBlurHints(QGraphicsBlurEffect::QualityHint);
    b_effect->setBlurRadius(50);
    playerBgImage = applyEffectToImage(playerBgImage, b_effect);
    WhatsMain->repaint();
}
void MainWindow::picRequestFinished(QNetworkReply* reply){
    QPixmap pix;
    PlayingImage = reply->readAll();
    pix.loadFromData(PlayingImage);
    ui->pushButton_2->setEnabled(true);
    if(ui->label->isHidden()){
        TempBtn2Icon = pix;
        changePlayingMask();
        ChangePlayerMainColor();
        listLyricsTime.clear();
        listLyricsText.clear();
        listTLyrics.clear();
        QNetworkRequest lrcRequest;
        QNetworkAccessManager* lrcNaManager = new QNetworkAccessManager(this);
        QMetaObject::Connection lrcConnRet = QObject::connect(lrcNaManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(lrcRequestFinished(QNetworkReply*)));
        Q_ASSERT(lrcConnRet);
        lrcRequest.setUrl(QUrl(NeteaseServer+"lyric?id="+QString::number(playUid)+"&cookie="+LoginCookie.toUtf8().toPercentEncoding()));
        lrcNaManager->get(lrcRequest);
    }
    else{
        ui->pushButton_2->setIcon(pix);
    }
}
QNetworkAccessManager M_qnam;
QByteArray MainWindow::Qget(const QString &strUrl)
{
    assert(!strUrl.isEmpty());

    const QUrl url = QUrl::fromUserInput(strUrl);
    assert(url.isValid());

    QNetworkRequest qnr(url);
    QNetworkReply* reply = M_qnam.get(qnr);

    QEventLoop eventLoop;
    connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec(QEventLoop::ExcludeUserInputEvents);
//    qDebug() << "status code= " + QString::number(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt());
    QByteArray replyData = reply->readAll();
    reply->deleteLater();
    reply = nullptr;

    return replyData;
}
void MainWindow::requestFinished(QNetworkReply* reply) {
    QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if(statusCode.isValid())
        qDebug() << "status code=" << statusCode.toInt();

    QVariant reason = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
    if(reason.isValid())
        qDebug() << "reason=" << reason.toString();

    QNetworkReply::NetworkError err = reply->error();
    if(err != QNetworkReply::NoError) {
        qFatal("Falied to read Music");
    }
    else {
        QJsonDocument document = QJsonDocument::fromJson(reply->readAll());
        if (!document.isNull()){
            if(document.isObject()){
                QJsonObject object = document.object();
                if(object.contains("data")){
                    QJsonValue value = object.value("data");
                    QJsonArray dataArray = value.toArray();
                    QJsonObject dataObject = dataArray[0].toObject();
                    if(dataObject.contains("time")){
                            ui->label_4->setText(formatTime(dataObject.value("time").toInt()));
                    }
                    if(dataObject.contains("url")){
                        QJsonValue Urlvalue = dataObject.value("url");
                        playUrl = Urlvalue.toString();
                        playUrl = "http://"+QUrl(playUrl).host()+QUrl(playUrl).path();
                        player->setMedia(playUrl);
                        player->play();
                        ui->pushButton->setIcon(QIcon(":/play.png"));
                    }
                }
            }
        }
    }
    }
MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::finishPlay(){
    ui->pushButton->setIcon(QIcon(":/stop.png"));
    player->stop();
    if(isRange == 0){
        if(!qPlayList->empty()){
            qPlayList->removeOne(playUid);
            if(!qPlayList->empty()){
                playUid = qPlayList->first();
                playMusic();
            }
            else{
                playUid = NULL;
            }
        }
    }
    else if(isRange == 1){
        if(!qPlayList->empty()){
            ui->pushButton->setIcon(QIcon(":/play.png"));
            player->play();
        }
    }
    else if(isRange == -1){
        if(!qPlayList->empty()){
            qPlayList->removeOne(playUid);
            if(!qPlayList->empty()){
                int i = QRandomGenerator::global()->bounded(qPlayList->count());
                qDebug() << i;
                playUid = qPlayList->at(i);
                playMusic();
            }
        }
    }
}
QString getLyricText(int index)
{
    if(index<0 || index>=listLyricsTime.size()){
        return "";
    }else{
        return listLyricsText[index];
    }
}
int getLyricTime(int index)
{
    if(index<0 || index>=listLyricsTime.size()){
        return 0;
    }else{
        return listLyricsTime[index];
    }
}
QString getTLyricText(int index,QString s)
{
    if(!listTLyrics.contains(index)){
        return "";
    }else if(s.isEmpty()){
        return "";
    }
    else{
        return listTLyrics.value(index);
    }
}
int lyricTextRows;
void changeLyricLabelText(int i){
    QString st = getLyricText(i-3);
    playUi->label_4->setText(st);
        playUi->label_13->setText(getTLyricText(getLyricTime(i-3),st));
        st = getLyricText(i-2);
    playUi->label_5->setText(st);
        playUi->label_12->setText(getTLyricText(getLyricTime(i-2),st));
        st = getLyricText(i-1);
    playUi->label_6->setText(st);
        playUi->label_11->setText(getTLyricText(getLyricTime(i-1),st));
        st = getLyricText(i);
    playUi->label_7->setText(st);
        playUi->label_10->setText(getTLyricText(getLyricTime(i),st));
        st = getLyricText(i+1);
    playUi->label_8->setText(st);
        playUi->label_9->setText(getTLyricText(getLyricTime(i+1),st));
}
bool isRowing = 0;
void MainWindow::changePlayBar(qint64 value){
    ui->horizontalSlider->setMinimum(0);
    ui->horizontalSlider->setMaximum(player->duration());
    ui->horizontalSlider->setValue(value);
    ui->label_3->setText(formatTime(value));
    if(player->duration() == value){
        if(isNotPlayOver == 0){
            finishPlay();
        }
        else{
            isNotPlayOver = 0;
        }
    }
    if(isRowing == 0){
        if(ui->label->isHidden() && listLyricsTime.count() != 0){
            int i=0;
            for (;i<listLyricsTime.count();i++) {
                if(listLyricsTime[i] >= value){
                    changeLyricLabelText(i);
                    lyricTextRows = i;
                    break;
                }
            }
        }
    }
}
void MainWindow::on_pushButton_clicked()
{
    if(playUid == NULL){
        return;
    }
    if(player->state() == QMediaPlayer::PlayingState){
        ui->pushButton->setIcon(QIcon(":/stop.png"));
        player->pause();
    }
    else{
        ui->pushButton->setIcon(QIcon(":/play.png"));
        player->play();
    }
}

void MainWindow::on_horizontalSlider_sliderMoved(int position)
{
    player->setPosition(position);
}
QList<QLabel*> searchPicLabels;
QList<QNetworkReply *> searchReply;
void MainWindow::getNetWork(const QList<QString> &strUrl)
{
    foreach (QString str, strUrl) {
        assert(!str.isEmpty());
        const QUrl url = QUrl::fromUserInput(str);
        assert(url.isValid());
        QNetworkAccessManager* m_qnam = new QNetworkAccessManager(this);
        QNetworkRequest qnr(url);
        searchReply << m_qnam->get(qnr);
        connect(m_qnam, SIGNAL(finished(QNetworkReply*)), this, SLOT(readGETthings(QNetworkReply*)));
    }
}
void MainWindow::readGETthings(QNetworkReply* reply){
    int i=0;
    foreach (QNetworkReply *searchreply, searchReply) {
     if(searchreply == reply){
         QPixmap pix;
         pix.loadFromData(reply->readAll());
         searchPicLabels[i]->setPixmap(pix);
         return;
     }
     i++;
    }
}
QList<int> searchSongs;
QWidget *searchUi;
QVBoxLayout *Scrolllayout;
int searchPages;
void MainWindow::on_pushButton_3_clicked()
{
    searchPages = 0;
    searchPage();
}
QSignalMapper * myMapper;
QSignalMapper * menuMapper;
QSignalMapper * downloadMapper;
void MainWindow::addList(QJsonArray searchArray,QLayout *layout){
    int i=searchSongs.count();
    QList<QString> PicUrlStr;
    foreach (QJsonValue searchResult, searchArray) {
        int searchId = searchResult.toObject().value("id").toInt();
        searchSongs.append(searchId);
        QWidget *SearchW = new QMusicTab(this);
        Ui::Form *formUi = new Ui::Form;
        formUi->setupUi(SearchW);
        QMenu* pMenu = new QMenu(this);
        formUi->pushButton_2->setMenu(pMenu);
        formUi->pushButton_2->setStyleSheet("QPushButton::menu-indicator{ image:none;}");
        QAction* pAction = new QAction(this);
        pMenu->addAction(pAction);
        pAction->setText("ÏÂÒ»Ê×²¥·Å");
        QAction* downAction = new QAction(this);
        pMenu->addAction(downAction);
        downAction->setText("ÏÂÔØ");
        QFontMetrics fontWidth(formUi->label_2->font());
        QString elideNote = fontWidth.elidedText(searchResult.toObject().value("name").toString(), Qt::ElideRight, 500);
        formUi->label_2->setText(elideNote);
        formUi->label_2->setToolTip(searchResult.toObject().value("name").toString());
        formUi->label->setText(searchResult.toObject().value("ar").toArray()[0].toObject().value("name").toString());
        QString picString = searchResult.toObject().value("al").toObject().value("picUrl").toString();
        picString = "http://"+QUrl(picString).host()+QUrl(picString).path()+"?param=64y64";
        PicUrlStr << picString;
        searchPicLabels.append(formUi->label_3);
        if(searchResult.toObject().value("fee").toInt() == 1){
            formUi->label_4->setText("VIP");
        }
        else if(searchResult.toObject().value("fee").toInt() == 4){
            formUi->label_4->setText("¸¶·Ñ");
        }
        QObject::connect(SearchW,SIGNAL(clicked()),myMapper, SLOT(map()));
        QObject::connect(pAction, SIGNAL(triggered()), menuMapper, SLOT(map()));
        QObject::connect(downAction, SIGNAL(triggered()), downloadMapper, SLOT(map()));
        myMapper->setMapping(SearchW, i);
        menuMapper->setMapping(pAction,i);
        downloadMapper->setMapping(downAction,i);
        layout->addWidget(SearchW);
        i++;
    }
    getNetWork(PicUrlStr);
}
void MainWindow::searchPage(){
    delete searchUi;
    delete Scrolllayout;
    myMapper = new QSignalMapper(this);
    menuMapper = new QSignalMapper(this);
    downloadMapper = new QSignalMapper(this);
    searchUi = new QWidget(this);
    Scrolllayout = new QVBoxLayout;
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setObjectName("scrollLayout");
    searchUi->setLayout(layout);
    ui->scrollAreaWidgetContents->setLayout(Scrolllayout);
    Scrolllayout->addWidget(searchUi);
    searchSongs.clear();
    QString searchEdit = ui->lineEdit->text();
    QNetworkRequest searchRequest;
    QNetworkAccessManager* searchNaManager = new QNetworkAccessManager(this);
    QMetaObject::Connection searchConnRet = QObject::connect(searchNaManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(searchRequestFinished(QNetworkReply*)));
    Q_ASSERT(searchConnRet);
    searchRequest.setUrl(QUrl(NeteaseServer+"/cloudsearch?keywords="+searchEdit+"&limit=10&offset="+QString::number(searchPages*10)+"&cookie="+LoginCookie.toUtf8().toPercentEncoding()));
    searchNaManager->get(searchRequest);
    connect(myMapper, SIGNAL(mapped(int)), this, SLOT(on_pushBtn_clicked(int)));
    connect(menuMapper, SIGNAL(mapped(int)), this, SLOT(nextPlaying(int)));
    connect(downloadMapper, SIGNAL(mapped(int)), this, SLOT(downloadMusic(int)));
    connect(ui->scrollArea->verticalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(finishScroll(int)));
}
void MainWindow::searchRequestFinished(QNetworkReply* reply){
    QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if(statusCode.isValid())
        qDebug() << "status code=" << statusCode.toInt();

    QVariant reason = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
    if(reason.isValid())
        qDebug() << "reason=" << reason.toString();

    QNetworkReply::NetworkError err = reply->error();
    if(err != QNetworkReply::NoError) {
        qFatal("Falied to read Music");
    }
    else {
        QJsonDocument document = QJsonDocument::fromJson(reply->readAll());
        if (!document.isNull()){
            if(document.isObject()){
                QJsonArray searchArray = document.object().value("result").toObject().value("songs").toArray();
                QVBoxLayout *layout = searchUi->findChild<QVBoxLayout*>("scrollLayout");
                addList(searchArray,layout);
            }
        }
    }
}
void MainWindow::downloadMusic(int i){
    qDebug() << searchSongs.at(i);
}
void MainWindow::finishScroll(int value){
    if(value == ui->scrollArea->verticalScrollBar()->maximum()){
        searchPages++;
        QString searchEdit = ui->lineEdit->text();
        QNetworkRequest searchRequest;
        QNetworkAccessManager* searchNaManager = new QNetworkAccessManager(this);
        QMetaObject::Connection searchConnRet = QObject::connect(searchNaManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(searchRequestFinished(QNetworkReply*)));
        Q_ASSERT(searchConnRet);
        searchRequest.setUrl(QUrl(NeteaseServer+"/cloudsearch?keywords="+searchEdit+"&limit=10&offset="+QString::number(searchPages*10)+"&cookie="+LoginCookie.toUtf8().toPercentEncoding()));
        searchNaManager->get(searchRequest);
    }
}
void MainWindow::on_pushBtn_clicked(int i){
    addPlayList(searchSongs.at(i),1);
}

void MainWindow::on_lineEdit_returnPressed()
{
    on_pushButton_3_clicked();
}
void MainWindow::addPlayList(int addId,bool isPlayNow){
    if(qPlayList->contains(addId)){
        qPlayList->removeOne(addId);
    }
    if(qPlayList->size() == 0){
        isPlayNow = 1;
    }
    qPlayList->append(addId);
    if(isPlayNow){
        playUid = addId;
        playMusic();
    }
    qDebug() << *qPlayList;
}

void MainWindow::on_horizontalSlider_2_sliderMoved(int position)
{
    player->setVolume(position);
    if(position <= 0){
        ui->pushButton_4->setIcon(QIcon(":/volumeMute.png"));
    }
    else if(position <= 30){
        ui->pushButton_4->setIcon(QIcon(":/volumeLow.png"));
    }
    else if(position <= 80){
        ui->pushButton_4->setIcon(QIcon(":/volumeMiddle.png"));
    }
    else{
        ui->pushButton_4->setIcon(QIcon(":/volumeHigh.png"));
    }
}


void MainWindow::on_pushButton_4_clicked()
{
    if(player->volume() != 0){
        player->setVolume(0);
        ui->pushButton_4->setIcon(QIcon(":/volumeMute.png"));
    }
    else{
        on_horizontalSlider_2_sliderMoved(ui->horizontalSlider_2->value());
    }
}


void MainWindow::on_toolButton_clicked()
{
    isRange++;
    if(isRange > 1){
        isRange = -1;
    }
    switch (isRange) {
    case -1:
        ui->toolButton->setIcon(QIcon(":/random.png"));
        break;
    case 0:
        ui->toolButton->setIcon(QIcon(":/range.png"));
        break;
    case 1:
        ui->toolButton->setIcon(QIcon(":/singalRange.png"));
        break;
    }
}


void MainWindow::on_pushButton_5_clicked()
{
    if(HomeBannerNow <= 0){
        HomeBannerNow = HomeBanner.count()-1;
    }
    else{
        HomeBannerNow--;
    }

    changeBanner(HomeBannerNow);
}


void MainWindow::on_pushButton_6_clicked()
{
    if(HomeBannerNow >= HomeBanner.count()-1){
        HomeBannerNow = 0;
    }
    else{
        HomeBannerNow++;
    }
    changeBanner(HomeBannerNow);
}

void MainWindow::nextPlaying(int i){
    addPlayList(searchSongs.at(i),0);
}
QWidget *playWidget;
void MainWindow::on_pushButton_2_clicked()
{
    if(ui->tabWidget->isHidden() == 1){
        ui->pushButton_2->setIcon(TempBtn2Icon);
        playWidget->hide();
        playWidget->deleteLater();
        listLyricsTime.clear();
        listLyricsText.clear();
        listTLyrics.clear();
        ui->tabWidget->show();
        ui->label->show();
        ui->label_2->show();
        this->repaint();
        return;
    }
    ui->tabWidget->hide();
    playWidget = new QPlaying(this);
    playUi = new Ui::QPlaying();
    playUi->setupUi(playWidget);
    ui->verticalLayout_5->insertWidget(0,playWidget);
    playUi->label_2->setText(ui->label_2->text());
    playUi->label_3->setText(ui->label->text());
    changePlayingMask();
    TempBtn2Icon = ui->pushButton_2->icon();
    ui->pushButton_2->setIcon(QIcon(":/up.png"));
    QNetworkRequest lrcRequest;
    QNetworkAccessManager* lrcNaManager = new QNetworkAccessManager(this);
    QMetaObject::Connection lrcConnRet = QObject::connect(lrcNaManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(lrcRequestFinished(QNetworkReply*)));
    Q_ASSERT(lrcConnRet);
    lrcRequest.setUrl(QUrl(NeteaseServer+"lyric?id="+QString::number(playUid)+"&cookie="+LoginCookie.toUtf8().toPercentEncoding()));
    lrcNaManager->get(lrcRequest);
    ChangePlayerMainColor();
    ResizePlayWidget(this->size());
    ui->label->hide();
    ui->label_2->hide();
    WhatsMain->repaint();
}
void MainWindow::ResizePlayWidget(QSize size){
    if(ui->label->isHidden()){
        int averageSize = (size.width()/3+size.height()/2)/2;
        playUi->label->setFixedSize(QSize(averageSize,averageSize));
        QFont ft;
        ft.setPointSize(size.height()/40);
            QFont ft_;
            ft_.setPointSize(ft.pointSize()/0.75);
            ft_.setBold(1);
            playUi->label_8->setFont(ft);
            playUi->label_7->setFont(ft);
            playUi->label_6->setFont(ft_);
            playUi->label_5->setFont(ft);
            playUi->label_4->setFont(ft);
            QFont _ft;
            _ft.setPointSize(ft.pointSize()*0.75);
            playUi->label_13->setFont(_ft);
            playUi->label_12->setFont(_ft);
            QFont _ft_ = QFont(_ft);
            _ft_.setBold(1);
            playUi->label_11->setFont(_ft_);
            playUi->label_10->setFont(_ft);
            playUi->label_9->setFont(_ft);
    }
}
bool analysisLyricsFile(QString line)
{
    if(line.isEmpty()){
        qDebug()<<"this line is empty!";
        return false;
    }
    QRegularExpression regularExpression("\\[(\\d+)?:(\\d+)?(\\.\\d+)?\\](.*)?");
    int index = 0;
    QRegularExpressionMatch match;
    match = regularExpression.match(line, index);
    if(match.hasMatch()) {
        int totalTime;
        totalTime = match.captured(1).toInt() * 60000 + match.captured(2).toInt() * 1000;
        QString currentText =QString::fromStdString(match.captured(4).toStdString());
        listLyricsTime.append(totalTime);
        listLyricsText.append(currentText);
        return true;
    }
    return false;
}
bool analysisTLyricsFile(QString line)
{
    if(line.isEmpty()){
        qDebug()<<"this line is empty!";
        return false;
    }
    QRegularExpression regularExpression("\\[(\\d+)?:(\\d+)?(\\.\\d+)?\\](.*)?");
    int index = 0;
    QRegularExpressionMatch match;
    match = regularExpression.match(line, index);
    if(match.hasMatch()) {
        int totalTime;
        totalTime = match.captured(1).toInt() * 60000 + match.captured(2).toInt() * 1000;
        QString currentText =QString::fromStdString(match.captured(4).toStdString());
        listTLyrics[totalTime] = currentText;
        return true;
    }
    return false;
}
void MainWindow::lrcRequestFinished(QNetworkReply* reply){
    QJsonDocument document = QJsonDocument::fromJson(reply->readAll());
    if (!document.isNull()){
        if(document.isObject()){
            QString lrc = document.object().value("lrc").toObject().value("lyric").toString();
            QStringList lines = lrc.split("\n");
            foreach (QString line, lines) {
                if(line == ""){
                    continue;
                }
                if(!analysisLyricsFile(line)){
                    qDebug() << "Can't read : " << line;
                }
            }
            if(document.object().value("tlyric").toObject().value("version").toInt() != 0){
                QString tlrc = document.object().value("tlyric").toObject().value("lyric").toString();
                QStringList lines = tlrc.split("\n");
                foreach (QString line, lines) {
                    if(line == ""){
                        continue;
                    }
                    if(!analysisTLyricsFile(line)){
                        qDebug() << "Can't read : " << line;
                    }
                }

            }
        }

    }
}
void MainWindow::resizeEvent(QResizeEvent * event)
{
    QSize curSize = event->size();
    ResizePlayWidget(curSize);
}
void QPlaying::wheelEvent(QWheelEvent *event)
{
    if(event->position().x() >= playUi->label_6->x()){
        int a = event->angleDelta().y();
        if(isRowing == 0){
            isRowing = 1;
            QTimer::singleShot(1000, WhatsMain, SLOT(stopWheel()));
        }
        else{
            if(a < 0){
                lyricTextRows++;
            }
            else{
                lyricTextRows--;
            }
        }

        if(a < 0){
            changeLyricLabelText(lyricTextRows+1);
        }else{
            changeLyricLabelText(lyricTextRows-1);
        }
    }
}

void MainWindow::stopWheel(){
    isRowing = 0;
}
Ui::NetEaseLogin *loginUi;
NetEaseLogin *loginWidget;
bool isLogin = 0;
void MainWindow::on_pushButton_7_clicked()
{
    if(!LoginCookie.isEmpty()){
        bool result=QMessageBox::question(this, "ÍË³ö","ÊÇ·ñÒªÍË³öµÇÂ¼?");
        if(result){
            LoginCookie = "";
            settings.setValue("Cookie",LoginCookie);
            settings.remove("profile");
            ui->label_9->setText("Î´µÇÂ¼");
        }
        return;
    }
    if(isLogin == 1){
        return;
    }
    loginWidget = new NetEaseLogin(this);
    loginWidget->setAttribute(Qt::WA_DeleteOnClose,1);
    loginWidget->setWindowFlags(Qt::Dialog);
    isLogin = 1;
    loginUi = new Ui::NetEaseLogin();
    loginUi->setupUi(loginWidget);
    connect(loginUi->pushButton,SIGNAL(clicked()),loginWidget,SLOT(close()));
    connect(loginUi->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(login_tab_changed(int)));
    connect(loginUi->pushButton_2,SIGNAL(clicked()),this,SLOT(login_pushbutton_clicked()));
    loginWidget->setWindowTitle("µÇÂ¼");
    loginWidget->show();
}
QString Qrkey;
QPixmap Base64_To_Image(QString bytearray)
{
    bytearray.remove(bytearray.split(",")[0]+",");
    QFile f(QDir::tempPath()+"/temp.png");
    if(f.exists()){
        f.remove();
    }
    f.open(QIODevice::WriteOnly);
    f.write(QByteArray::fromBase64(bytearray.toUtf8()));
    f.close();
    f.open(QIODevice::ReadOnly);
    QPixmap imageresult;
    imageresult.loadFromData(f.readAll());
    f.close();
    return imageresult;
}
void MainWindow::login_tab_changed(int i){
    if(i == 1){
        Qrkey = QJsonDocument::fromJson(Qget(NeteaseServer+"login/qr/key")).object().value("data").toObject().value("unikey").toString();
        QJsonDocument qrCode = QJsonDocument::fromJson(Qget(NeteaseServer+"login/qr/create?key="+Qrkey.toUtf8()+"&qrimg=true"));
        QPixmap pic = Base64_To_Image(qrCode.object().value("data").toObject().value("qrimg").toString());
        pic = pic.scaled(loginUi->label->size());
        loginUi->label->setPixmap(pic);
        QJsonDocument qrState = QJsonDocument::fromJson(Qget(NeteaseServer+"login/qr/check?key="+Qrkey));
        while(!(qrState.object().value("code").toInt() == 803)){
            QCoreApplication::processEvents(QEventLoop::AllEvents, 5);
            QDateTime time = QDateTime::currentDateTime();
            qrState = QJsonDocument::fromJson(Qget(NeteaseServer+"login/qr/check?key="+Qrkey+"&time="+QString::number(time.toTime_t())));
            if(isLogin == 0){
                return;
            }
            if(WhatsMain->isHidden()){
                QCoreApplication::quit();
                return;
            }
        }
        LoginCookie = qrState.object().value("cookie").toString();
        settings.setValue("Cookie",LoginCookie);
        QJsonDocument document = QJsonDocument::fromJson(Qget(NeteaseServer+"user/account?cookie="+LoginCookie.toUtf8().toPercentEncoding()));
        settings.setValue("profile",document.object().value("profile").toObject());
        ui->label_9->setText(document.object().value("profile").toObject().value("nickname").toString());
        loginWidget->close();
    }
}
void NetEaseLogin::closeEvent(QCloseEvent*){
    isLogin = 0;
}
void MainWindow::login_pushbutton_clicked(){
    QNetworkRequest loginRequest;
    QNetworkAccessManager* loginNaManager = new QNetworkAccessManager(this);
    QMetaObject::Connection loginConnRet = QObject::connect(loginNaManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(loginRequestFinished(QNetworkReply*)));
    Q_ASSERT(loginConnRet);
    QDateTime time = QDateTime::currentDateTime();
    if(loginUi->lineEdit->text().contains("@")){
        loginRequest.setUrl(QUrl(NeteaseServer+"login?email="+loginUi->lineEdit->text()+"&password="+loginUi->lineEdit_2->text()+"&time="+QString::number(time.toTime_t())+"&realIP="+localhostName));
    }
    else{
        loginRequest.setUrl(QUrl(NeteaseServer+"login/cellphone?phone="+loginUi->lineEdit->text()+"&password="+loginUi->lineEdit_2->text()+"&time="+QString::number(time.toTime_t())+"&realIP="+localhostName));
    }
    loginNaManager->get(loginRequest);
}
void MainWindow::loginRequestFinished(QNetworkReply* reply){
    QJsonDocument document = QJsonDocument::fromJson(reply->readAll());
    if (!document.isNull()){
        if(document.isObject()){
            if(document.object().value("code").toInt() == 200){
                LoginCookie = document.object().value("cookie").toString();
                settings.setValue("Cookie",LoginCookie);
                settings.setValue("profile",document.object().value("profile").toObject());
                ui->label_9->setText(document.object().value("profile").toObject().value("nickname").toString());
                loginWidget->close();
            }
            else if(document.object().value("code").toInt() == 460){
                qDebug() << "µÇÂ½Ê§°Ü: " << document;
                QMessageBox::information(this,"µÇÂ¼Ê§°Ü£¡code:"+QString::number(document.object().value("code").toInt()),"Çë³¢ÊÔÊ¹ÓÃÆäËû·½Ê½µÇÂ¼");
            }
            else{
                qDebug() << "µÇÂ½Ê§°Ü: " << document;
                QMessageBox::information(this,"µÇÂ¼Ê§°Ü£¡code:"+QString::number(document.object().value("code").toInt()),"Çë¼ì²éÊäÈëÊÇ·ñÕýÈ·");
            }
        }
    }
}
void addPlayList(QLayout *layout){
    NeteasePlayList *listWidget = new NeteasePlayList(WhatsMain);
    Ui::NeteasePlayList *listUi = new Ui::NeteasePlayList;
    listUi->setupUi(listWidget);
    layout->addWidget(listWidget);
}
void MainWindow::on_tabWidget_2_currentChanged(int index)
{
    if(index == 1 && !LoginCookie.isEmpty()){
        if(ui->tab_5->layout()){
            delete(ui->tab_5->layout());
        }
        myMapper = new QSignalMapper(this);
        menuMapper = new QSignalMapper(this);
        downloadMapper = new QSignalMapper(this);
        connect(myMapper, SIGNAL(mapped(int)), this, SLOT(on_pushBtn_clicked(int)));
        connect(menuMapper, SIGNAL(mapped(int)), this, SLOT(nextPlaying(int)));
        connect(downloadMapper, SIGNAL(mapped(int)), this, SLOT(downloadMusic(int)));
        QVBoxLayout *layout = new QVBoxLayout;
        ui->tab_5->setLayout(layout);
        addPlayList({},layout);
    }
    else if(index == 1){
        ui->tabWidget_2->setCurrentIndex(0);
    }
}

