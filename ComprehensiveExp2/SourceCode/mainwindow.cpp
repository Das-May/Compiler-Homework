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

    connect(ui->Btn_ImportBNF,&QPushButton::clicked,[=](){
            QString FileName = QFileDialog::getOpenFileName(this,QStringLiteral("选取BNF文法"),"D:",QStringLiteral("文本文件(*.txt)"));
            if(FileName != "")
            {
                QByteArray fn_qba = FileName.toUtf8();
                char* fn_cc = fn_qba.data();
                char* c = Loader::ReadText(fn_cc);

                ui->TE_BNF->setText(QString(c));

                GrammarProcessorInstance = new GrammarProcessor(c);

                ui->TE_Simplified->setText(QString::fromStdString(GrammarProcessorInstance->SimplifyGrammar()));

                ui->TE_RemoveLeftCommonFactor->setText(QString::fromStdString(GrammarProcessorInstance->RemoveLeftCommonFactor()));

                ui->TE_RemoveLeftRecursion->setText(QString::fromStdString(GrammarProcessorInstance->RemoveLeftRecursion()));

                ui->TE_FirstSet->setText(QString::fromStdString(GrammarProcessorInstance->GetFirst()));

                ui->TE_FollowSet->setText(QString::fromStdString(GrammarProcessorInstance->GetFollow()));
            }
        }
    );

    connect(ui->Btn_ImportCode,&QPushButton::clicked,[=](){

        }
    );
}

MainWindow::~MainWindow()
{
    delete ui;
}

