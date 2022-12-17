#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QString>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);



    connect(ui->btn,&QPushButton::clicked,[=](){
        QString s = ui->lineEdit->text();
        this->re.GetNFA(s.toStdString());


    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

