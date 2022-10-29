#ifndef NETEASELOGIN_H
#define NETEASELOGIN_H

#include <QWidget>
#include <QCloseEvent>

namespace Ui {
class NetEaseLogin;
}

class NetEaseLogin : public QWidget
{
    Q_OBJECT

public:
    explicit NetEaseLogin(QWidget *parent = nullptr);
    void closeEvent(QCloseEvent *event);
};

#endif // NETEASELOGIN_H
