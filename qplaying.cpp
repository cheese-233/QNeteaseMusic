#include "qplaying.h"
#include "ui_qplaying.h"

QPlaying::QPlaying(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QPlaying)
{
    ui->setupUi(this);
}

QPlaying::~QPlaying()
{
    delete ui;
}
