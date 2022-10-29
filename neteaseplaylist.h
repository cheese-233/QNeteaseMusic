#ifndef NETEASEPLAYLIST_H
#define NETEASEPLAYLIST_H

#include <QWidget>

namespace Ui {
class NeteasePlayList;
}

class NeteasePlayList : public QWidget
{
    Q_OBJECT

public:
    explicit NeteasePlayList(QWidget *parent = nullptr);


protected:
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
signals:
    void clicked();
private:
    QPoint mousePos;

private:
    Ui::NeteasePlayList *ui;
};

#endif // NETEASEPLAYLIST_H
