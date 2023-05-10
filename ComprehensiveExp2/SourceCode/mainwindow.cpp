#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "Loader.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QString>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    {
    QString FileName = "D:\\Homework\\Compiler-Homework\\build-Qt_Comprehensive_Exp2-Desktop_Qt_5_12_9_MinGW_32_bit-Debug\\a.txt";
    QByteArray fn_qba = FileName.toUtf8();
    char* fn_cc = fn_qba.data();
    char* c = Loader::ReadText(fn_cc);
    ui->TE_BNF->setText(QString(c));
    qDebug() << "=== Init ===\n";
    GrammarProcessorInstance = new GrammarProcessor(c);
    qDebug() << "=== Simplify ===\n";
    ui->TE_Simplified->setText(QString::fromStdString(GrammarProcessorInstance->SimplifyGrammar()));
    qDebug() << "=== Remove Left Common Factor ===\n";
    ui->TE_RemoveLeftCommonFactor->setText(QString::fromStdString(GrammarProcessorInstance->RemoveLeftCommonFactor()));
    qDebug() << "=== Remove Left Recursion ===\n";
    ui->TE_RemoveLeftRecursion->setText(QString::fromStdString(GrammarProcessorInstance->RemoveLeftRecursion()));
    qDebug() << "=== GetFirst ===\n";
    ui->TE_FirstSet->setText(QString::fromStdString(GrammarProcessorInstance->GetFirst()));
    qDebug() << "=== GetFollow ===\n";
    ui->TE_FollowSet->setText(QString::fromStdString(GrammarProcessorInstance->GetFollow()));
    qDebug() << "=== LL1 ===\n";
    SetLL1Table();
    }

    connect(ui->Btn_ImportBNF,&QPushButton::clicked,[=](){
            QString FileName = QFileDialog::getOpenFileName(this,QStringLiteral("选取BNF文法"),"D:",QStringLiteral("文本文件(*.txt)"));
            if(FileName != "")
            {
                QByteArray fn_qba = FileName.toUtf8();
                char* fn_cc = fn_qba.data();
                char* c = Loader::ReadText(fn_cc);

                ui->TE_BNF->setText(QString(c));
                qDebug() << "=== Init ===\n";
                GrammarProcessorInstance = new GrammarProcessor(c);
                qDebug() << "=== Simplify ===\n";
                ui->TE_Simplified->setText(QString::fromStdString(GrammarProcessorInstance->SimplifyGrammar()));
                qDebug() << "=== Remove Left Common Factor ===\n";
                ui->TE_RemoveLeftCommonFactor->setText(QString::fromStdString(GrammarProcessorInstance->RemoveLeftCommonFactor()));
                qDebug() << "=== Remove Left Recursion ===\n";
                ui->TE_RemoveLeftRecursion->setText(QString::fromStdString(GrammarProcessorInstance->RemoveLeftRecursion()));
                qDebug() << "=== GetFirst ===\n";
                ui->TE_FirstSet->setText(QString::fromStdString(GrammarProcessorInstance->GetFirst()));
                qDebug() << "=== GetFollow ===\n";
                ui->TE_FollowSet->setText(QString::fromStdString(GrammarProcessorInstance->GetFollow()));

                //GrammarProcessorInstance->GetLL1Table(ui->TW_LL1);
            }
        }
    );

    {
    QString FileName = "D:\\Homework\\Compiler-Homework\\toGithub\\ComprehensiveExp2\\TestData\\SAMPLE1.TNY";
    QByteArray fn_qba = FileName.toUtf8();
    char* fn_cc = fn_qba.data();
    char* c = Loader::ReadText(fn_cc);
    ui->TE_Code->setText(QString(c));
    }
    connect(ui->Btn_ImportCode,&QPushButton::clicked,[=](){

        }
    );
}

void MainWindow::SetLL1Table()
{
    vector<vector<int>> LL1Table = GrammarProcessorInstance->GetLL1Table();
    map<int, string> ID2Word = GrammarProcessorInstance->GetID2Word();

    int row = LL1Table[0][0];
    int col = LL1Table[0][1];

    ui->TW_LL1->setRowCount(row);
    ui->TW_LL1->setColumnCount(col);
    for(int i = 1; i < col; i++)
        ui->TW_LL1->setItem(0, i, new QTableWidgetItem(QString::fromStdString(ID2Word[LL1Table[i][0]])));

    for(int i = col; i < row*col; i++)
    {
        if(i%col==0)
            ui->TW_LL1->setItem(i/col, 0, new QTableWidgetItem(QString::fromStdString(ID2Word[LL1Table[i][0]])));
        else
        {
            if(LL1Table[i].size()>1)
            {
                string rule = ID2Word[LL1Table[i][0]] + " -> ";
                for(int j = 1; j < LL1Table[i].size(); j++)
                    rule += ID2Word[LL1Table[i][j]] + ' ';
                ui->TW_LL1->setItem(i/col, i%col, new QTableWidgetItem(QString::fromStdString(rule)));
            }
        }
    }
}

void MainWindow::GetGrammarTree()
{

}

MainWindow::~MainWindow()
{
    delete ui;
}

