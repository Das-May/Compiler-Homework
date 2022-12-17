#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <qtextcodec.h>
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

    //QString fn = "D://Homework//Compiler-Homework//build-Qt_Compiler4-Desktop_Qt_5_12_9_MinGW_32_bit-Debug//testcase7.txt";

    connect(ui->openBtn,&QPushButton::clicked,[=](){
        QString fileName = QFileDialog::getOpenFileName(this,QStringLiteral("读取TINY语言代码"),"D:",QStringLiteral("tiny源码(*TNY)"));
        if(fileName != "")
        {
            // 读入源代码
            QTextCodec *code = QTextCodec::codecForName("GB2312");//解决中文路径问题
            QByteArray fn_qba = code->fromUnicode(fileName);
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
    connect(ui->compileBtn,&QPushButton::clicked,[=](){
        tiny.execute(ui->sourceText->toPlainText().toLatin1().data());

        string temp = tiny.GetOutput();
        ui->OutputTex->setText(QString::fromStdString(temp));

        temp = tiny.GetsyntaxTree();
        ui->TreeTex->setText(QString::fromStdString(temp));

    });

    connect(ui->saveBtn,&QPushButton::clicked,[=](){
        QString fileName = QFileDialog::getSaveFileName(this,QStringLiteral("保存TINY语言代码"),"D:",QStringLiteral("tiny源码(*TNY)"));
        if(fileName != "")
        {
            QTextCodec *code = QTextCodec::codecForName("GB2312");
            QByteArray fn_qba = code->fromUnicode(fileName);
            char* fn_cc = fn_qba.data();
            Loader::SetFilepath(fn_cc);

            QByteArray code_qba = ui->sourceText->toPlainText().toLatin1();
            char* code_cc = code_qba.data();
            Loader::Output(code_cc);

            QMessageBox::information(this,"提示", "保存成功！文件路径为"+QString(Loader::GetFilepath()));

        }
    });

}


MainWindow::~MainWindow()
{
    delete ui;
}

