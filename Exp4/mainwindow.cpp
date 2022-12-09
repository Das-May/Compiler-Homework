#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>

#include "Loader.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("TINY语言扩充");

    connect(ui->openBtn,&QPushButton::clicked,[=](){
        QString fileName = QFileDialog::getOpenFileName(this,QStringLiteral("读取TINY语言代码"),"D:",QStringLiteral("(*txt)"));
        if(fileName != "")
        {
            // 读入源代码
            QByteArray fn_qba = fileName.toLatin1();
            char* fn_cc = fn_qba.data();
            char* c = Loader::Input(fn_cc);

            ui->sourceText->setText(QString::fromStdString(c));

            string temp;

        }
    });

    connect(ui->saveBtn,&QPushButton::clicked,[=](){

    });

}


MainWindow::~MainWindow()
{
    delete ui;
}

