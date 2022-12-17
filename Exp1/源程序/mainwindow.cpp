#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("文本压缩工具");

    connect(ui->compressBtn,&QPushButton::clicked,[=](){
        QString fileName = QFileDialog::getOpenFileName(this,QStringLiteral("选取代码文件"),"C:",QStringLiteral("(*cpp *c)"));
        if(fileName != "")
            compress(fileName);
    });

    connect(ui->decompressBtn,&QPushButton::clicked,[=](){
        QString fileName = QFileDialog::getOpenFileName(this,QStringLiteral("选取压缩文件"),"C:",QStringLiteral("(*bin)"));
        if(fileName != "")
            decompress(fileName);
    });
}

void MainWindow::compress(QString fileName)
{
    // 读取代码
    QByteArray fn_qba = fileName.toLatin1(); // must
    char* fn_cc = fn_qba.data();
    char* c = loader.Input(fn_cc);
    if(c == 0)
    {
        QMessageBox::warning(this, "警告", "文件打开失败！");
        return;
    }
    ui->label1->setText(QString(c));
    // 压缩代码
    c = handler.compress(c);
    if(c == 0)
    {
        QMessageBox::warning(this, "警告", "代码文件中，不可出现除了注释以外的中文！");
        return;
    }
    ui->label2->setText(QString(c));
    loader.Output(c);
    QMessageBox::information(this,"提示", "压缩成功！文件路径为"+QString(loader.GetFilepath()));
}

void MainWindow::decompress(QString fileName)
{
    QByteArray fn_qba = fileName.toLatin1(); // must
    char* fn_cc = fn_qba.data();
    char* c = loader.Input(fn_cc);
    if(c == 0)
    {
        QMessageBox::warning(this, "警告", "文件打开失败！");
        return;
    }
    ui->label1->setText(QString(c));
    c = handler.decompress(c);
    ui->label2->setText(QString(c));
    loader.Output(c);
    QMessageBox::information(this,"提示", "解压成功！文件路径为"+QString(loader.GetFilepath()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

