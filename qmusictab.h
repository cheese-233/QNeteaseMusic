#ifndef QMUSICTAB_H
#define QMUSICTAB_H

#include <QWidget>
#include <QMouseEvent>

class QMusicTab : public QWidget
{
    Q_OBJECT
public:
    explicit QMusicTab(QWidget *parent = nullptr);

signals:
    void clicked();
protected:
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);

private:
    QPoint mousePos;

};

#endif // QMUSICTAB_H
