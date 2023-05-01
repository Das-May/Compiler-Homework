#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "Loader.h"
#include <QFileDialog>
#include <QMessageBox>

#include <QDebug>

#include <Regex2NFA.h>
#include <Automata.h>

#include <string>

#define TW_NFA(index) ui->TW_NFA_##index
#define TW_DFA(index) ui->TW_DFA_##index
#define TW_MinDFA(index) ui->TW_MinDFA_##index

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("编码解析器");

    QString FileName = "D:\\Homework\\Compiler-Homework\\toGithub\\ComprehensiveExp1\\TestData\\TINY.txt";
    QByteArray fn_qba = FileName.toUtf8();
    char* fn_cc = fn_qba.data();
    char* c = Loader::ReadText(fn_cc);

    ProcessRegex(c);

//    connect(ui->Btn_OpenText,&QPushButton::clicked,[=](){
//        QString FileName = QFileDialog::getOpenFileName(this,QStringLiteral("选取正则表达式.txt"),"D:",QStringLiteral("(*txt)"));
//        if(FileName != "")
//        {
//            QByteArray fn_qba = FileName.toUtf8();
//            char* fn_cc = fn_qba.data();
//            char* c = Loader::ReadText(fn_cc);

//            ProcessRegex(c);
//        }
//    }
//    );

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

bool IsSeparator(char c)
{
    return (c==' ' || c=='\n' || c=='\r' || c=='\t');
}

void PassSeparator(char *&c)
{
    while(IsSeparator(*c))
        c++;
}

string GetFeild(char *&c)
{
    string temp = "";
    while (!IsSeparator(*c))
    {
        temp += *c;
        c++;
    }
    return temp;
}

void DisplayFATable(QTableWidget*& TW, const vector<vector<int>>& FA)
{
    if(FA.size() > 200)
        return;

    int Row = FA[0][0];
    int Col = FA[0][1];

    TW->setRowCount(Row);
    TW->setColumnCount(Col);

    for(int i = 1; i < Col; i++)
        TW->setItem(0, i, new QTableWidgetItem(QString((char)FA[i][0])));

    for(int i = Col; i < Row*Col; i++)
    {
        string item = "";
        bool IsFirst = false;
        for(int j : FA[i])
        {
            if(!IsFirst)
            {
                IsFirst = true;
                item += to_string(j);
            }
            else
                item += ',' + to_string(j);
        }
        TW->setItem(i/Col, i%Col, new QTableWidgetItem(QString::fromStdString(item)));
    }
}

void MainWindow::ProcessRegex(char* c)
{
    TypeCount = 0;
    string regex;
    Regex2NFA regex2NFA;
    vector<vector<int>> NFATable;
    vector<vector<int>> DFATable;
    vector<vector<int>> MinDFATable;
    while(*c)
    {
        PassSeparator(c);

        TypeName[TypeCount] = GetFeild(c);
        ui->tabWidget->setTabText(TypeCount, QString::fromStdString(TypeName[TypeCount]));

        PassSeparator(c);

        regex = GetFeild(c);
        qDebug() << QString::fromStdString(regex) << "is being processing." << endl;

        while(regex.find("letter")!= string::npos)
            regex.replace(regex.find("letter"), 6, "(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z)");
        while(regex.find("digit") != string::npos)
            regex.replace(regex.find("digit"), 5, "(0|1|2|3|4|5|6|7|8|9)");

        NFATable = regex2NFA.GetNFA(regex);

        DFATable = Automata::GetDFA(NFATable);

//        MinDFATable = Automata::GetMinDFA(DFATable);
//        MinDFA[TypeCount] = MinDFATable;

        // 以下是屎山，不要看
        switch (TypeCount)
        {
            case 0:
                DisplayFATable(ui->TW_NFA_0, NFATable);
                DisplayFATable(ui->TW_DFA_0, DFATable);
//                DisplayFATable(ui->TW_MinDFA_0, MinDFATable);
                break;
            case 1:
                DisplayFATable(ui->TW_NFA_1, NFATable);
                DisplayFATable(ui->TW_DFA_1, DFATable);
//                DisplayFATable(ui->TW_MinDFA_1, MinDFATable);
                break;
            case 2:
                DisplayFATable(ui->TW_NFA_2, NFATable);
                DisplayFATable(ui->TW_DFA_2, DFATable);
//                DisplayFATable(ui->TW_MinDFA_2, MinDFATable);
                break;
            case 3:
                DisplayFATable(ui->TW_NFA_3, NFATable);
                DisplayFATable(ui->TW_DFA_3, DFATable);
//                DisplayFATable(ui->TW_MinDFA_3, MinDFATable);
                break;
            case 4:
                DisplayFATable(ui->TW_NFA_4, NFATable);
                DisplayFATable(ui->TW_DFA_4, DFATable);
//                DisplayFATable(ui->TW_MinDFA_4, MinDFATable);
                break;
            case 5:
                DisplayFATable(ui->TW_NFA_5, NFATable);
                DisplayFATable(ui->TW_DFA_5, DFATable);
//                DisplayFATable(ui->TW_MinDFA_5, MinDFATable);
                break;
            default:
                qDebug() << "too much types of keyword~" << endl;
        }

        qDebug() << QString::fromStdString(regex) << "finished." << endl;
        TypeCount++;

        PassSeparator(c);
    }
}


MainWindow::~MainWindow()
{
    delete ui;
}

