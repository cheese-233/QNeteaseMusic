#ifndef QPLAYING_H
#define QPLAYING_H

#include <QWidget>

namespace Ui {
class QPlaying;
}

class QPlaying : public QWidget
{
    Q_OBJECT

public:
    explicit QPlaying(QWidget *parent = nullptr);

protected:
    void wheelEvent(QWheelEvent *event);
};

#endif // QPLAYING_H
