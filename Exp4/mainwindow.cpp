#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QString>

#include "Loader.h"
#include "tiny.h"

Tiny tiny;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("TINY语言扩充");

    // 读入源代码
    QString fn = "D://Homework//Compiler-Homework//build-Qt_Compiler4-Desktop_Qt_5_12_9_MinGW_32_bit-Debug//testcase1.txt";
    QByteArray fn_qba = fn.toLatin1();
    char* fn_cc = fn_qba.data();
    char* c = Loader::Input(fn_cc);

    ui->sourceText->setText(QString::fromStdString(c));

    tiny.execute(c);

    string temp = tiny.GetOutput();
    ui->OutputTex->setText(QString::fromStdString(temp));

    temp = tiny.GetsyntaxTree();
    ui->TreeTex->setText(QString::fromStdString(temp));

    connect(ui->openBtn,&QPushButton::clicked,[=](){
        QString fileName = QFileDialog::getOpenFileName(this,QStringLiteral("读取TINY语言代码"),"D:",QStringLiteral("(*txt)"));
        if(fileName != "")
        {
            // 读入源代码
            QByteArray fn_qba = fileName.toLatin1();
            char* fn_cc = fn_qba.data();
            char* c = Loader::Input(fn_cc);

            ui->sourceText->setText(QString::fromStdString(c));

            tiny.execute(c);

            string temp = tiny.GetOutput();
            ui->OutputTex->setText(QString::fromStdString(temp));

            temp = tiny.GetsyntaxTree();
            ui->TreeTex->setText(QString::fromStdString(temp));

        }
    });

    connect(ui->saveBtn,&QPushButton::clicked,[=](){

    });

}


MainWindow::~MainWindow()
{
    delete ui;
}

