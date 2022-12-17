#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("文法问题处理器");

    //QString fileName = "D://Homework//Compiler-Homework//build-Qt_Compiler3-Desktop_Qt_5_12_9_MinGW_32_bit-Debug//data2.txt";

    connect(ui->btn,&QPushButton::clicked,[=](){
        QString fileName = QFileDialog::getOpenFileName(this,QStringLiteral("选取代码文件"),"D:",QStringLiteral("(*txt)"));
        if(fileName != "")
        {
            ui->lineEdit->setText(fileName);

            // 读入文法
            QByteArray fn_qba = fileName.toLatin1();
            char* fn_cc = fn_qba.data();
            char* c = Loader::Input(fn_cc);
            gm = new GrammarProcessor(c);
            ui->textEdit->setText(QString::fromStdString(gm->PrintGrammar()));

            string temp;

            // 化简文法
            temp = gm->SimplifyGrammar();
            ui->Simplify_Lable->setText(QString::fromStdString(temp));

            // 消除左公因子
            temp = gm->RemoveLeftCommonFactor();
            ui->leftCF_Label->setText(QString::fromStdString(temp));

            // 消除左递归
            temp = gm->RemoveLeftRecursion();
            ui->leftR_Label->setText(QString::fromStdString(temp));

            // 获取First集
            temp = gm->GetFirst();
            ui->First_Lable->setText(QString::fromStdString(temp));

            // 获取Follow集
            temp = gm->GetFollow();
            ui->Follow_Lable->setText(QString::fromStdString(temp));

            re = new RegularExpressionProcessor();
            re->SetNFA(gm->GetNFA());

            // 获取NFA
            temp = re->GetNFA();
            ui->NFA_Lable->setText(QString::fromStdString(temp));

            // 获取DFA
            temp = re->GetDFA();
            ui->DFA_Lable->setText(QString::fromStdString(temp));

            // 获取最小DFA
            temp = re->GetMinDFA();
            ui->minDFA_Lable->setText(QString::fromStdString(temp));


        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

