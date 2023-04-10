#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "Loader.h"
#include <QFileDialog>
#include <QMessageBox>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("编码解析器");

    connect(ui->Btn_OpenText,&QPushButton::clicked,[=](){
        QString FileName = QFileDialog::getOpenFileName(this,QStringLiteral("选取正则表达式.txt"),"D:",QStringLiteral("(*txt)"));
        if(FileName != "")
        {
            QByteArray fn_qba = FileName.toUtf8();
            char* fn_cc = fn_qba.data();
            char* c = Loader::ReadText(fn_cc);


        }
    }
    );

    connect(ui->Btn_SaveCode,&QPushButton::clicked,[=](){
        QString FileName = QFileDialog::getOpenFileName(this,QStringLiteral("保存路径"),"D:",QStringLiteral("(*txt)"));
        if(FileName != "")
        {
            QByteArray fn_qba = FileName.toUtf8();
            char* fn_cc = fn_qba.data();
            //Loader::WriteText()


        }
    }
    );
}

MainWindow::~MainWindow()
{
    delete ui;
}

