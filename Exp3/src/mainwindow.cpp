#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("文法问题处理器");

    connect(ui->btn,&QPushButton::clicked,[=](){
        QString fileName = QFileDialog::getOpenFileName(this,QStringLiteral("选取代码文件"),"D:",QStringLiteral("(*txt)"));
        if(fileName != "")
        {
            QByteArray fn_qba = fileName.toLatin1(); // must
            char* fn_cc = fn_qba.data();
            char* c = Loader::Input(fn_cc);

            ui->lineEdit->setText(fileName);

            gm = new GrammarProcessor(c);
            ui->textEdit->setText(QString::fromStdString(gm->PrintGrammar()));

        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

