#ifndef LOGIN_H
#define LOGIN_H
#include "cameracalibration.h"
#include <QDialog>
#include <QCloseEvent>
namespace Ui {
class Login;
}

class Login : public QDialog
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = 0);
    ~Login();
    QString user_online;

private slots:
    void on_loginBtn_clicked();

    void on_exitBtn_clicked();
protected:
    void closeEvent(QCloseEvent *event);
private:
    Ui::Login *ui;
};

#endif // LOGIN_H
