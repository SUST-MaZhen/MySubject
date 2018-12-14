#include "login.h"
#include "dialog.h"
#include "ui_login.h"
#include <QMessageBox>
#include <QIcon>
Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    setWindowIcon(QIcon("icon.ico"));
    this->setFixedSize( this->width (),this->height ());
    //this->setStyleSheet("background-color:pink");
    ui->passwdLineEdit->setEchoMode(QLineEdit::Password);//以密码形式输入
    QString button_style="QPushButton{background-color:green;color: white; border-radius:5px;  border: 2px groove gray; border-style: outset;}"
                         "QPushButton:hover{background-color:white; color: black;}"
                         "QPushButton:pressed{background-color:rgb(85, 170, 255); border-style: inset; }";
    this->setStyleSheet(button_style);
}

Login::~Login()
{
    delete ui;
}

void Login::on_loginBtn_clicked()
{   
    this->accept();
    Dialog d("马振");
    d.show();
    d.exec();
}

void Login::on_exitBtn_clicked()
{

    QMessageBox mess(QMessageBox::NoIcon, "close", "exit now ?",
                     QMessageBox::Yes | QMessageBox::No, NULL);
    if(mess.exec() == QMessageBox::Yes)
        accept();
    else
    {
        ui->userLineEdit->setFocus();
        return;
    }
}

void Login::closeEvent(QCloseEvent *event)
{
    int ret = QMessageBox::information( this, tr("close"),
      tr("exit now ?"),
      tr("Yes"), tr("No"));
    if(ret == 0)
        event->accept();
    if(ret == 1)
    {
        event->ignore();
        ui->userLineEdit->setFocus();
    }

}
